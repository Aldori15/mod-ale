#ifndef PLAYERBOTMETHODS_H
#define PLAYERBOTMETHODS_H

#include "ALEIncludes.h"
#include "CharacterCache.h"

/***
 * Represents Playerbots/RNDBots for servers running mod-playerbots
 */
namespace LuaGlobalBot
{
    /**
     * Creates a bot [WorldSession] without a network socket (isBot=true).
     * The session is added to the world session manager but no character is logged in yet.
     * Use [LoginBotByGuid] to log in a character on this session.
     *
     * You typically don't need to call this directly — [LoginBotByGuid] creates its own session.
     *
     * @param uint32 accountId : the account ID for the bot session
     * @return bool success : true if session was created, false if it already exists or accountId is invalid
     */
    int CreateBotSession(lua_State* L)
    {
        uint32 accountId = ALE::CHECKVAL<uint32>(L, 1);

        if (!accountId)
        {
            ALE::Push(L, false);
            return 1;
        }

        if (eWorldSessionMgr->FindSession(accountId))
        {
            ALE::Push(L, false);
            return 1;
        }

        WorldSession* session = new WorldSession(
            accountId, "", 0x0, nullptr, SEC_PLAYER,
            EXPANSION_WRATH_OF_THE_LICH_KING,
            time_t(0), sWorld->GetDefaultDbcLocale(),
            0, false, false, 0, true
        );

        eWorldSessionMgr->AddSession(session);

        ALE::Push(L, true);
        return 1;
    }

    /**
     * Creates a new bot character in the database and returns its GUID (low part).
     * The character is created via the standard Player::Create() and saved via SaveToDB().
     * After creation, use [LoginBotByGuid] to log the character into the world.
     *
     * Note: SaveToDB() uses async queries internally. If you call [LoginBotByGuid] immediately
     * after, the character data may not be flushed to the database yet. Use a short timer delay.
     *
     * @param uint32 accountId : the account to create the character on
     * @param string name : character name
     * @param uint8 race : race ID (e.g. 1 = Human, 2 = Orc)
     * @param uint8 class : class ID (e.g. 1 = Warrior, 2 = Paladin)
     * @param uint8 gender : 0 = Male, 1 = Female
     * @param uint8 skin = 0 : skin color index
     * @param uint8 face = 0 : face type index
     * @param uint8 hairStyle = 0 : hair style index
     * @param uint8 hairColor = 0 : hair color index
     * @param uint8 facialHair = 0 : facial hair type index
     * @return uint32 guidLow : the low part of the new character's GUID, or nil on failure
     */
    int CreateBotPlayer(lua_State* L)
    {
        uint32 accountId  = ALE::CHECKVAL<uint32>(L, 1);
        std::string name  = ALE::CHECKVAL<std::string>(L, 2);
        uint8 race        = ALE::CHECKVAL<uint8>(L, 3);
        uint8 cls         = ALE::CHECKVAL<uint8>(L, 4);
        uint8 gender      = ALE::CHECKVAL<uint8>(L, 5);
        uint8 skin        = ALE::CHECKVAL<uint8>(L, 6, 0);
        uint8 face        = ALE::CHECKVAL<uint8>(L, 7, 0);
        uint8 hairStyle   = ALE::CHECKVAL<uint8>(L, 8, 0);
        uint8 hairColor   = ALE::CHECKVAL<uint8>(L, 9, 0);
        uint8 facialHair  = ALE::CHECKVAL<uint8>(L, 10, 0);

        if (!accountId || name.empty())
            return 0;

        WorldSession* tempSession = new WorldSession(
            accountId, "", 0x0, nullptr, SEC_PLAYER,
            EXPANSION_WRATH_OF_THE_LICH_KING,
            time_t(0), sWorld->GetDefaultDbcLocale(),
            0, false, false, 0, true
        );

        std::unique_ptr<CharacterCreateInfo> createInfo =
            std::make_unique<CharacterCreateInfo>(
                name, race, cls, gender, skin, face,
                hairStyle, hairColor, facialHair
            );

        Player* player = new Player(tempSession);
        player->GetMotionMaster()->Initialize();

        ObjectGuid::LowType guidLow = sObjectMgr->GetGenerator<HighGuid::Player>().Generate();

        if (!player->Create(guidLow, createInfo.get()))
        {
            player->CleanupsBeforeDelete();
            delete player;
            delete tempSession;
            return 0;
        }

        player->setCinematic(2);
        player->SetAtLoginFlag(AT_LOGIN_NONE);
        player->SaveToDB(true, false);

        sCharacterCache->AddCharacterCacheEntry(
            player->GetGUID(), accountId, player->GetName(),
            player->getGender(), player->getRace(),
            player->getClass(), player->GetLevel()
        );

        uint32 result = player->GetGUID().GetCounter();

        player->CleanupsBeforeDelete();
        delete player;
        delete tempSession;

        ALE::Push(L, result);
        return 1;
    }

    /**
     * Logs in an existing character as a bot (ASYNCHRONOUS).
     * Creates a WorldSession with isBot=true and loads the character from the database
     * via DelayQueryHolder. The bot will appear in the world after a few server ticks.
     *
     * If the character is already logged in, returns the existing [Player] object immediately.
     * If async loading has started, returns true.
     * Returns nil on error.
     *
     * To detect when the bot finishes loading, register a PLAYER_EVENT_ON_LOGIN hook
     * and check [Player]:IsBot().
     *
     * @param uint32 accountId : the account ID that owns the character
     * @param uint32 guidLow : the low part of the character's GUID
     * @return [Player]|bool|nil : [Player] if already in world, true if async load started, nil on error
     */
    int LoginBotByGuid(lua_State* L)
    {
        uint32 accountId = ALE::CHECKVAL<uint32>(L, 1);
        uint32 guidLow   = ALE::CHECKVAL<uint32>(L, 2);

        if (!accountId || !guidLow)
            return 0;

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);

        Player* existing = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (existing && existing->IsInWorld())
        {
            ALE::Push(L, existing);
            return 1;
        }

        std::shared_ptr<LoginQueryHolder> holder =
            std::make_shared<LoginQueryHolder>(accountId, playerGuid);

        if (!holder->Initialize())
            return 0;

        sWorld->AddQueryHolderCallback(CharacterDatabase.DelayQueryHolder(holder))
            .AfterComplete(
                [accountId](SQLQueryHolderBase const& queryHolder)
                {
                    LoginQueryHolder const& holder =
                        static_cast<LoginQueryHolder const&>(queryHolder);

                    WorldSession* botSession = new WorldSession(
                        accountId, "", 0x0, nullptr, SEC_PLAYER,
                        EXPANSION_WRATH_OF_THE_LICH_KING,
                        time_t(0), sWorld->GetDefaultDbcLocale(),
                        0, false, false, 0, true
                    );

                    botSession->HandlePlayerLoginFromDB(holder);

                    Player* bot = botSession->GetPlayer();
                    if (!bot)
                    {
                        botSession->LogoutPlayer(true);
                        delete botSession;
                        return;
                    }
                });

        ALE::Push(L, true);
        return 1;
    }

    /**
     * Logs out a bot by its GUID.
     * Calls WorldSession::LogoutPlayer and destroys the bot session.
     * Only works on bot sessions (IsBot=true). Will not log out real players.
     *
     * @param uint32 guidLow : the low part of the bot character's GUID
     * @return bool success : true if the bot was logged out, false if not found or not a bot
     */
    int LogoutBotByGuid(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);

        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        WorldSession* session = bot->GetSession();
        if (!session || !session->IsBot())
        {
            ALE::Push(L, false);
            return 1;
        }

        session->LogoutPlayer(true);
        delete session;

        ALE::Push(L, true);
        return 1;
    }

    /**
     * Finds a logged-in player (including bots) by GUID.
     * Returns the [Player] object if found and in world, nil otherwise.
     *
     * @param uint32 guidLow : the low part of the character's GUID
     * @return [Player]|nil player : the player object or nil
     */
    int FindBotPlayer(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);

        Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (player && player->IsInWorld())
            ALE::Push(L, player);
        else
            ALE::Push(L);
        return 1;
    }

    /**
     * Returns the total number of active world sessions (both real players and bots).
     *
     * @return uint32 count : active session count
     */
    int GetBotSessionCount(lua_State* L)
    {
        ALE::Push(L, eWorldSessionMgr->GetActiveSessionCount());
        return 1;
    }

    /**
     * Creates a new game account.
     * Uses the core AccountMgr to register a username/password pair in the login database.
     *
     * @param string username : account username
     * @param string password : account password
     * @return uint32 result : AccountOpResult enum (0 = AOR_OK, see AccountMgr.h for other codes)
     */
    int CreateBotAccount(lua_State* L)
    {
        std::string username = ALE::CHECKVAL<std::string>(L, 1);
        std::string password = ALE::CHECKVAL<std::string>(L, 2);

        AccountOpResult result = AccountMgr::CreateAccount(username, password);
        ALE::Push(L, static_cast<uint32>(result));
        return 1;
    }

    /**
     * Returns the account ID for a given username.
     *
     * @param string username : account username to look up
     * @return uint32 accountId : the account ID, or 0 if not found
     */
    int GetAccountIdByUsername(lua_State* L)
    {
        std::string username = ALE::CHECKVAL<std::string>(L, 1);
        ALE::Push(L, AccountMgr::GetId(username));
        return 1;
    }

    /**
     * Returns the number of characters on an account.
     *
     * @param uint32 accountId : the account ID
     * @return uint32 count : number of characters
     */
    int GetAccountCharCount(lua_State* L)
    {
        uint32 accountId = ALE::CHECKVAL<uint32>(L, 1);
        ALE::Push(L, AccountMgr::GetCharactersCount(accountId));
        return 1;
    }

    /**
     * Deletes an account and all its characters.
     *
     * @param uint32 accountId : the account ID to delete
     * @return uint32 result : AccountOpResult enum (0 = AOR_OK)
     */
    int DeleteBotAccount(lua_State* L)
    {
        uint32 accountId = ALE::CHECKVAL<uint32>(L, 1);
        AccountOpResult result = AccountMgr::DeleteAccount(accountId);
        ALE::Push(L, static_cast<uint32>(result));
        return 1;
    }

    /**
     * Returns the GUID (low part) of a character by name.
     * Looks up the character cache, not the database directly.
     *
     * @param string name : character name
     * @return uint32 guidLow : the low part of the character's GUID, or 0 if not found
     */
    int GetCharGuidByName(lua_State* L)
    {
        std::string name = ALE::CHECKVAL<std::string>(L, 1);
        ObjectGuid guid = sCharacterCache->GetCharacterGuidByName(name);
        ALE::Push(L, guid.GetCounter());
        return 1;
    }

    /**
     * Returns the account ID that owns a character, looked up by GUID.
     *
     * @param uint32 guidLow : the low part of the character's GUID
     * @return uint32 accountId : the owning account ID, or 0 if not found
     */
    int GetCharAccountIdByGuid(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        ALE::Push(L, sCharacterCache->GetCharacterAccountIdByGuid(guid));
        return 1;
    }

    /**
     * Returns the character name for a given GUID.
     *
     * @param uint32 guidLow : the low part of the character's GUID
     * @return string name : the character name, or empty string if not found
     */
    int GetCharNameByGuid(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        std::string name;
        sCharacterCache->GetCharacterNameByGuid(guid, name);
        ALE::Push(L, name);
        return 1;
    }

};
#endif
