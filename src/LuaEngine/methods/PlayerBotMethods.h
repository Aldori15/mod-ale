#ifndef PLAYERBOTMETHODS_H
#define PLAYERBOTMETHODS_H

#include "ALEIncludes.h"
#include "CharacterCache.h"
#include "PlayerbotMgr.h"
#include "PlayerbotAI.h"

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

    /**
     * Returns the current master (owner/controller) of a bot.
     *
     * For a normal player-owned bot, this is the player controlling it.
     * For a self-bot, this may return the bot/player itself.
     * For an unowned random bot, this may return nil.
     *
     * @param uint32 guidLow : bot GUID low
     * @return [Player]|nil master : the current master player, or nil if none
     */
    int GetBotMaster(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L);
            return 1;
        }

        ALE::Push(L, ai->GetMaster());
        return 1;
    }

    /**
     * Send a remote command to a bot and return the response string.
     *
     * Sample commands (non-exhaustive; see PlayerbotAI::HandleRemoteCommand):
     *  - "state"    : returns "combat", "dead", or "non-combat"
     *  - "position" : returns "x y z mapId orientation [|zone name|]"
     *  - "tpos"     : target's position "x y z mapId orientation" (requires a current target)
     *  * "movement" : last movement data
     *  - "target"   : current target's name
     *  - "hp"       : bot HP% (and target HP% if any)
     *  - "strategy" : list active strategies
     *  - "action"   : last executed action
     *  - "values"   : formatted AI values
     *  - "travel"   : travel target/status
     *  - "budget"   : money/budget overview
     *
     * Example:
     *   local res = SendBotCommand(guidLow, "position")
     *
     * @param uint32 guidLow : bot GUID low
     * @param string command : command text
     * @return string response : response text or nil on error
     */
    int SendBotCommand(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string command = ALE::CHECKVAL<std::string>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L);
            return 1;
        }

        std::string res = ai->HandleRemoteCommand(command);
        ALE::Push(L, res);
        return 1;
    }

    /**
     * Execute a specific action on a bot.
     *
     * This calls `PlayerbotAI::DoSpecificAction(actionName, Event(), false, qualifier)`.
     * Action names are the same as those used by mod-playerbots' action/strategy system.
     * Sample action names:
     *  - "follow", "stay", "sit", "loot", "mount", "dismount", "attack", "melee"
     *  - "use <item>", "cast <spell>", "buy", "sell", "accept quest", "turn in petition"
     *  - "food", "drink", "healing potion", "mana potion", "heal", "flee", "travel"
     *
     * See modules/mod-playerbots/src/Ai/Base/ActionContext.h for a more complete list.
     *
     * Example:
     *   local ok = DoBotAction(guidLow, "follow")
     *   local ok = DoBotAction(guidLow, "cast Fireball", "targetname")
     *
     * @param uint32 guidLow : bot GUID low
     * @param string actionName : action to perform
     * @param string qualifier = "" : optional qualifier (depends on action)
     * @return bool success
     */
    int DoBotAction(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string actionName = ALE::CHECKVAL<std::string>(L, 2);
        std::string qualifier = ALE::CHECKVAL<std::string>(L, 3, std::string());

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }

        bool ok = ai->DoSpecificAction(actionName, Event(), false, qualifier);
        ALE::Push(L, ok);
        return 1;
    }

    /**
     * Send a message to the bot's master (private / tell).
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @return bool success
     */
    int BotTellMaster(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }
        ALE::Push(L, ai->TellMaster(text));
        return 1;
    }

    /**
     * Make the bot say text in say-range chat.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @return bool success
     */
    int BotSay(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }
        ALE::Push(L, ai->Say(text));
        return 1;
    }

    /**
     * Make the bot yell text in yell-range chat.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @return bool success
     */
    int BotYell(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }
        ALE::Push(L, ai->Yell(text));
        return 1;
    }

    /**
     * Make the bot whisper text to a specific player name.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @param string receiver : recipient player name
     * @return bool success
     */
    int BotWhisper(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);
        std::string receiver = ALE::CHECKVAL<std::string>(L, 3);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }
        ALE::Push(L, ai->Whisper(text, receiver));
        return 1;
    }

    /**
     * Send an error message to the bot's master (used by AI to report errors).
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : error text
     * @return bool success
     */
    int BotTellError(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }
        ALE::Push(L, ai->TellError(text));
        return 1;
    }

    /**
     * Check whether the bot can cast a given spell on an optional target.
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32 spellId : spell ID
     * @param Unit|nil target : optional target unit
     * @return bool canCast : true if the bot can cast the spell
     */
    int BotCanCastSpell(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        uint32 spellId = ALE::CHECKVAL<uint32>(L, 2);
        Unit* target = ALE::CHECKOBJ<Unit>(L, 3, false);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }

        ALE::Push(L, ai->CanCastSpell(spellId, target, true));
        return 1;
    }

    /**
     * Command the bot to cast a spell on a target.
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32 spellId : spell ID
     * @param Unit|nil target : optional target unit
     * @return bool success : true if cast was initiated
     */
    int BotCastSpell(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        uint32 spellId = ALE::CHECKVAL<uint32>(L, 2);
        Unit* target = ALE::CHECKOBJ<Unit>(L, 3, false);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }

        ALE::Push(L, ai->CastSpell(spellId, target));
        return 1;
    }

    /**
     * Check whether the bot has an aura.
     *
     * The second parameter may be a numeric `spellId` or a `spellName` string.
     * An optional third parameter can specify a `Unit` to check (defaults to the bot).
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32|string spellIdOrName : spell id or name
     * @param Unit|nil target : optional unit to check
     * @return bool hasAura
     */
    int BotHasAura(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }

        if (lua_isnumber(L, 2))
        {
            uint32 spellId = ALE::CHECKVAL<uint32>(L, 2);
            Unit* player = ALE::CHECKOBJ<Unit>(L, 3, false);
            ALE::Push(L, ai->HasAura(spellId, player));
            return 1;
        }
        else
        {
            std::string spellName = ALE::CHECKVAL<std::string>(L, 2);
            Unit* player = ALE::CHECKOBJ<Unit>(L, 3, false);
            ALE::Push(L, ai->GetAura(spellName, player) != nullptr);
            return 1;
        }
    }

    /**
     * Remove an aura by name from the bot.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string name : aura/spell name to remove
     * @return bool success
     */
    int BotRemoveAura(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string name = ALE::CHECKVAL<std::string>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }
        ai->RemoveAura(name);
        ALE::Push(L, true);
        return 1;
    }

    /**
     * Find a consumable item in the bot's inventory by itemId.
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32 itemId : item template ID to search for
     * @return [Item]|nil item : the Item object if found, or nil
     */
    int FindBotConsumable(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        uint32 itemId = ALE::CHECKVAL<uint32>(L, 2);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L);
            return 1;
        }
        Item* it = ai->FindConsumable(itemId);
        ALE::Push(L, it);
        return 1;
    }

    /**
     * Return a formatted list of bots belonging to `master`.
     *
     * @param Player master : master player object
     * @return string list : formatted list (implementation-defined)
     */
    int ListBotsByMaster(lua_State* L)
    {
        Player* master = ALE::CHECKOBJ<Player>(L, 1);
        PlayerbotMgr* mgr = sPlayerbotsMgr.GetPlayerbotMgr(master);
        if (!mgr)
        {
            ALE::Push(L, std::string());
            return 1;
        }
        ALE::Push(L, mgr->ListBots(master));
        return 1;
    }

    /**
     * Return the total number of playerbots managed for the provided `master`.
     *
     * @param Player master : master player object
     * @return uint32 count
     */
    int GetPlayerbotsCount(lua_State* L)
    {
        Player* master = ALE::CHECKOBJ<Player>(L, 1);
        PlayerbotMgr* mgr = sPlayerbotsMgr.GetPlayerbotMgr(master);
        if (!mgr)
        {
            ALE::Push(L, (uint32)0);
            return 1;
        }
        ALE::Push(L, mgr->GetPlayerbotsCount());
        return 1;
    }

    /**
     * Return the number of playerbots of a specific class for the provided `master`.
     *
     * @param Player master : master player object
     * @param uint32 cls : class ID
     * @return uint32 count
     */
    int GetPlayerbotsCountByClass(lua_State* L)
    {
        Player* master = ALE::CHECKOBJ<Player>(L, 1);
        uint32 cls = ALE::CHECKVAL<uint32>(L, 2);
        PlayerbotMgr* mgr = sPlayerbotsMgr.GetPlayerbotMgr(master);
        if (!mgr)
        {
            ALE::Push(L, (uint32)0);
            return 1;
        }
        ALE::Push(L, mgr->GetPlayerbotsCountByClass(cls));
        return 1;
    }

    /**
     * Trigger account-linking for a master via PlayerbotMgr.
     *
     * This calls the PlayerbotMgr handler which performs linking and sends feedback
     * messages to the `master`. No value is returned to Lua.
     *
     * @param Player master
     * @param string accountName
     * @param string key
     */
    int LinkAccount(lua_State* L)
    {
        Player* master = ALE::CHECKOBJ<Player>(L, 1);
        std::string accountName = ALE::CHECKVAL<std::string>(L, 2);
        std::string key = ALE::CHECKVAL<std::string>(L, 3);
        PlayerbotMgr* mgr = sPlayerbotsMgr.GetPlayerbotMgr(master);
        if (!mgr)
            return 0;
        mgr->HandleLinkAccountCommand(master, accountName, key);
        return 0;
    }

    /**
     * Trigger unlinking an account from the master via PlayerbotMgr.
     * No value is returned to Lua.
     *
     * @param Player master
     * @param string accountName
     */
    int UnlinkAccount(lua_State* L)
    {
        Player* master = ALE::CHECKOBJ<Player>(L, 1);
        std::string accountName = ALE::CHECKVAL<std::string>(L, 2);
        PlayerbotMgr* mgr = sPlayerbotsMgr.GetPlayerbotMgr(master);
        if (!mgr)
            return 0;
        mgr->HandleUnlinkAccountCommand(master, accountName);
        return 0;
    }

    /**
     * Trigger viewing linked accounts for the master via PlayerbotMgr.
     * The manager will send the result to the `master`. No Lua return value.
     *
     * @param Player master
     */
    int ViewLinkedAccounts(lua_State* L)
    {
        Player* master = ALE::CHECKOBJ<Player>(L, 1);
        PlayerbotMgr* mgr = sPlayerbotsMgr.GetPlayerbotMgr(master);
        if (!mgr)
            return 0;
        mgr->HandleViewLinkedAccountsCommand(master);
        return 0;
    }

    /**
     * Find a candidate new master for a bot.
     *
     * @param uint32 guidLow : bot GUID low
     * @return [Player]|nil newMaster : candidate player or nil
     */
    int FindNewMaster(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L);
            return 1;
        }
        ALE::Push(L, ai->FindNewMaster());
        return 1;
    }

    /**
     * Return true if the bot has a real player as its master.
     *
     * This differs from `IsRealPlayer()`: a normal player-owned bot returns
     * true here, but false for `IsRealPlayer()`. RNDBots typically return false.
     *
     * @param uint32 guidLow : bot GUID low
     * @return bool
     */
    int HasRealPlayerMaster(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->HasRealPlayerMaster() : false);
        return 1;
    }

    /**
     * Return true if this Player is treated as a real player by mod-playerbots.
     *
     * In mod-playerbots, this is only true for a self-bot, where the AI's
     * `master` pointer is the same Player as the bot itself (`master == bot`).
     *
     * This means:
     * - true  : a player running PlayerbotAI on their own character
     * - false : an RNDBot / random bot
     * - false : a bot controlled by some other player
     *
     * If you want to know whether a bot has a real player owner, use the
     * related player-master checks instead.
     *
     * @param uint32 guidLow : bot GUID low
     * @return bool
     */
    int IsRealPlayer(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->IsRealPlayer() : false);
        return 1;
    }

    /**
     * Return true if the bot is considered an Altbot rather than a random bot.
     *
     * In mod-playerbots this is true when the bot has a real player master and
     * is not flagged as a random bot.
     *
     * @param uint32 guidLow : bot GUID low
     * @return bool
     */
    int IsAltbot(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->IsAlt() : false);
        return 1;
    }

    /**
     * Return the group leader for the bot, or its master when no group leader is available.
     *
     * @param uint32 guidLow : bot GUID low
     * @return [Player]|nil leader
     */
    int GetGroupLeader(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L);
            return 1;
        }

        ALE::Push(L, ai->GetGroupLeader());
        return 1;
    }

    /**
     * Return the bot's current AI state.
     *
     * State values are the `BotState` enum from mod-playerbots:
     *  - 0 = combat
     *  - 1 = non-combat
     *  - 2 = dead
     *
     * @param uint32 guidLow : bot GUID low
     * @return uint32 state
     */
    int GetBotState(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, static_cast<uint32>(BOT_STATE_NON_COMBAT));
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? static_cast<uint32>(ai->GetState())
                        : static_cast<uint32>(BOT_STATE_NON_COMBAT));
        return 1;
    }

    /**
     * Return the active strategies for a bot state as a Lua array of strings.
     *
     * State values are the `BotState` enum from mod-playerbots:
     *  - 0 = combat
     *  - 1 = non-combat
     *  - 2 = dead
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32 state : BotState value
     * @return table strategies
     */
    int GetBotStrategies(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        uint32 state = ALE::CHECKVAL<uint32>(L, 2, static_cast<uint32>(BOT_STATE_NON_COMBAT));

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            lua_newtable(L);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            lua_newtable(L);
            return 1;
        }

        std::vector<std::string> strategies = ai->GetStrategies(static_cast<BotState>(state));
        lua_createtable(L, static_cast<int>(strategies.size()), 0);
        for (size_t index = 0; index < strategies.size(); ++index)
        {
            ALE::Push(L, strategies[index]);
            lua_rawseti(L, -2, static_cast<int>(index + 1));
        }

        return 1;
    }

    /**
     * Check whether a named strategy is active for the specified bot state.
     *
     * Strategy names are the same names used internally by mod-playerbots.
     * Valid names depend on the bot's class/spec and on the selected state.
     *
     * Common examples:
     *  - Non-combat: "nc", "follow", "chat", "default", "quest", "loot", "food", "mount"
     *  - Combat: "dps", "heal", "tank", "aoe", "dps assist", "tank assist", "cure"
     *  - Class/spec examples: "fire", "frost", "arcane", "holy", "shadow", "resto",
     *    "enh", "affli", "demo", "destro", "blood", "unholy", "bm", "mm", "surv"
     *  - Dead state examples: "dead", "stay", "follow", "chat"
     *
     * Use `GetBotStrategies` to inspect the currently active strategies for a
     * given state, or `SendBotCommand(guidLow, "strategy")` to view the bot's
     * current strategy listing.
     *
     * State values are the `BotState` enum:
     *  - 0 = combat
     *  - 1 = non-combat
     *  - 2 = dead
     *
     * @param uint32 guidLow : bot GUID low
     * @param string strategyName : strategy name to test
     * @param uint32 state = 1 : BotState value
     * @return bool
     */
    int HasBotStrategy(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string strategyName = ALE::CHECKVAL<std::string>(L, 2);
        uint32 state = ALE::CHECKVAL<uint32>(L, 3, static_cast<uint32>(BOT_STATE_NON_COMBAT));

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->HasStrategy(strategyName, static_cast<BotState>(state)) : false);
        return 1;
    }

    /**
     * Toggle a named strategy for the specified bot state.
     *
     * This forwards to `PlayerbotAI::ChangeStrategy`. In mod-playerbots this
     * toggles the named strategy on or off depending on whether it is already active.
     * Valid strategy names depend on the bot's class/spec and state.
     *
     * Common examples:
     *  - Non-combat: "follow", "quest", "loot", "chat", "food", "mount"
     *  - Combat: "dps", "heal", "tank", "aoe", "cure", "dps assist", "tank assist"
     *  - Class/spec examples: "fire", "frost", "arcane", "shadow", "holy", "resto",
     *    "enh", "blood", "unholy", "affli", "demo", "destro", "bm", "mm", "surv"
     *
     * Use `GetBotStrategies` first if you want to see what is currently active.
     * If you are unsure whether a name is valid for the bot, inspect the class
     * strategy setup in mod-playerbots or use `SendBotCommand(guidLow, "strategy")`.
     *
     * State values are the `BotState` enum:
     *  - 0 = combat
     *  - 1 = non-combat
     *  - 2 = dead
     *
     * @param uint32 guidLow : bot GUID low
     * @param string strategyName : strategy name
     * @param uint32 state = 1 : BotState value
     * @return bool success
     */
    int ChangeBotStrategy(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string strategyName = ALE::CHECKVAL<std::string>(L, 2);
        uint32 state = ALE::CHECKVAL<uint32>(L, 3, static_cast<uint32>(BOT_STATE_NON_COMBAT));

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }

        ai->ChangeStrategy(strategyName, static_cast<BotState>(state));
        ALE::Push(L, true);
        return 1;
    }

    /**
     * Make the bot leave its current group, or disband it if appropriate.
     *
     * @param uint32 guidLow : bot GUID low
     * @return bool success
     */
    int LeaveOrDisbandBotGroup(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        if (!ai)
        {
            ALE::Push(L, false);
            return 1;
        }

        ai->LeaveOrDisbandGroup();
        ALE::Push(L, true);
        return 1;
    }

    /**
     * Return the number of nearby group members within the given distance.
     *
     * @param uint32 guidLow : bot GUID low
     * @param float distance : search distance in yards
     * @return int32 count
     */
    int GetNearGroupMemberCount(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        float distance = ALE::CHECKVAL<float>(L, 2, sPlayerbotAIConfig.sightDistance);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, static_cast<int32>(0));
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->GetNearGroupMemberCount(distance) : static_cast<int32>(0));
        return 1;
    }

    /**
     * Check whether the bot has at least one copy of an item in its inventory.
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32 itemId : item template ID
     * @return bool
     */
    int HasBotItemInInventory(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        uint32 itemId = ALE::CHECKVAL<uint32>(L, 2);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->HasItemInInventory(itemId) : false);
        return 1;
    }

    /**
     * Return how many copies of an item the bot has in its inventory.
     *
     * @param uint32 guidLow : bot GUID low
     * @param uint32 itemId : item template ID
     * @return uint32 count
     */
    int GetBotInventoryItemCount(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        uint32 itemId = ALE::CHECKVAL<uint32>(L, 2);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, static_cast<uint32>(0));
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->GetInventoryItemsCountWithId(itemId) : static_cast<uint32>(0));
        return 1;
    }

    /**
     * Make the bot speak in party chat.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @return bool success
     */
    int BotSayToParty(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->SayToParty(text) : false);
        return 1;
    }

    /**
     * Make the bot speak in guild chat.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @return bool success
     */
    int BotSayToGuild(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->SayToGuild(text) : false);
        return 1;
    }

    /**
     * Make the bot speak in raid chat.
     *
     * @param uint32 guidLow : bot GUID low
     * @param string text : message text
     * @return bool success
     */
    int BotSayToRaid(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        std::string text = ALE::CHECKVAL<std::string>(L, 2);

        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }

        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->SayToRaid(text) : false);
        return 1;
    }

    /**
     * Return true if the bot currently has a connected real player as its master.
     *
     * This is useful for checking whether the bot is actively owned/controlled
     * by a live player right now. It is typically false for RNDBots, bots with
     * no master, and self-bots.
     *
     * @param uint32 guidLow : bot GUID low
     * @return bool
     */
    int HasActivePlayerMaster(lua_State* L)
    {
        uint32 guidLow = ALE::CHECKVAL<uint32>(L, 1);
        ObjectGuid playerGuid = ObjectGuid::Create<HighGuid::Player>(guidLow);
        Player* bot = ObjectAccessor::FindConnectedPlayer(playerGuid);
        if (!bot)
        {
            ALE::Push(L, false);
            return 1;
        }
        PlayerbotAI* ai = sPlayerbotsMgr.GetPlayerbotAI(bot);
        ALE::Push(L, ai ? ai->HasActivePlayerMaster() : false);
        return 1;
    }

};
#endif
