/*
* Copyright (C) 2010 - 2025 Eluna Lua Engine <https://elunaluaengine.github.io/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef CREATUREMETHODS_H
#define CREATUREMETHODS_H

/***
 * Non-[Player] controlled [Unit]s (i.e. NPCs).
 *
 * Inherits all methods from: [Object], [WorldObject], [Unit]
 */
namespace LuaCreature
{
    /**
     * Returns `true` if the [Creature] can regenerate health,
     *   and returns `false` otherwise.
     *
     * @return bool isRegenerating
     */
    int IsRegeneratingHealth(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->isRegeneratingHealth());
        return 1;
    }

    /**
     * Sets whether the [Creature] can regenerate health or not.
     *
     * @param bool enable = true : `true` to enable health regeneration, `false` to disable it
     */
    int SetRegeneratingHealth(lua_State* L, Creature* creature)
    {
        bool enable = ALE::CHECKVAL<bool>(L, 2, true);

        creature->SetRegeneratingHealth(enable);
        return 0;
    }

    /**
     * Returns `true` if the [Creature] is set to not give reputation when killed,
     *   and returns `false` otherwise.
     *
     * @return bool reputationDisabled
     */
    int IsReputationGainDisabled(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsReputationRewardDisabled());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] completes the [Quest] with the ID `questID`,
     *   and returns `false` otherwise.
     *
     * @param uint32 questID : the ID of a [Quest]
     * @return bool completesQuest
     */
    int CanCompleteQuest(lua_State* L, Creature* creature)
    {
        uint32 quest_id = ALE::CHECKVAL<uint32>(L, 2);

        ALE::Push(L, creature->hasInvolvedQuest(quest_id));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can be targeted for attack,
     *   and returns `false` otherwise.
     *
     * @param bool mustBeDead = false : if `true`, only returns `true` if the [Creature] is also dead. Otherwise, it must be alive.
     * @return bool targetable
     */
    int IsTargetableForAttack(lua_State* L, Creature* creature)
    {
        bool mustBeDead = ALE::CHECKVAL<bool>(L, 2, false);

        ALE::Push(L, creature->isTargetableForAttack(mustBeDead));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can assist `friend` in combat against `enemy`,
     *   and returns `false` otherwise.
     *
     * @param [Unit] friend : the Unit we will be assisting
     * @param [Unit] enemy : the Unit that we would attack if we assist `friend`
     * @param bool checkFaction = true : if `true`, the [Creature] must be the same faction as `friend` to assist
     * @return bool canAssist
     */
    int CanAssistTo(lua_State* L, Creature* creature)
    {
        Unit* u = ALE::CHECKOBJ<Unit>(L, 2);
        Unit* enemy = ALE::CHECKOBJ<Unit>(L, 3);
        bool checkfaction = ALE::CHECKVAL<bool>(L, 4, true);

        ALE::Push(L, creature->CanAssistTo(u, enemy, checkfaction));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] has searched for combat assistance already,
     *   and returns `false` otherwise.
     *
     * @return bool searchedForAssistance
     */
    int HasSearchedAssistance(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->HasSearchedAssistance());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] will give its loot to `player`,
     *   and returns `false` otherwise.
     *
     * @return bool tapped
     */
    int IsTappedBy(lua_State* L, Creature* creature)
    {
        Player* player = ALE::CHECKOBJ<Player>(L, 2);

        ALE::Push(L, creature->isTappedBy(player));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] will give its loot to a [Player] or [Group],
     *   and returns `false` otherwise.
     *
     * @return bool hasLootRecipient
     */
    int HasLootRecipient(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->hasLootRecipient());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can start attacking nearby hostile [Unit]s,
     *   and returns `false` otherwise.
     *
     * @return bool canAggro
     */
    int CanAggro(lua_State* L, Creature* creature)
    {
        ALE::Push(L, !creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can move through deep water,
     *   and returns `false` otherwise.
     *
     * @return bool canSwim
     */
    int CanSwim(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->CanSwim());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can move on land,
     *   and returns `false` otherwise.
     *
     * @return bool canWalk
     */
    int CanWalk(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->CanWalk());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is returning to its spawn position from combat,
     *   and returns `false` otherwise.
     *
     * @return bool inEvadeMode
     */
    int IsInEvadeMode(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsInEvadeMode());
        return 1;
    }

    /**
     * Returns `true` if the [Creature]'s rank is Elite or Rare Elite,
     *   and returns `false` otherwise.
     *
     * @return bool isElite
     */
    int IsElite(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->isElite());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is a city guard,
     *   and returns `false` otherwise.
     *
     * @return bool isGuard
     */
    int IsGuard(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsGuard());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is a civilian,
     *   and returns `false` otherwise.
     *
     * @return bool isCivilian
     */
    int IsCivilian(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsCivilian());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is the leader of a player faction,
     *   and returns `false` otherwise.
     *
     * @return bool isLeader
     */
    int IsRacialLeader(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsRacialLeader());
        return 1;
    }

    /**
     * Returns `true` if the [Creature]'s flags_extra includes Dungeon Boss (0x1000000),
     *   and returns `false` otherwise.
     *
     * @return bool isDungeonBoss
     */
    int IsDungeonBoss(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsDungeonBoss());
        return 1;
    }

    /**
     * Returns `true` if the [Creature]'s rank is Boss,
     *   and returns `false` otherwise.
     *
     * @return bool isWorldBoss
     */
    int IsWorldBoss(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->isWorldBoss());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] cannot cast `spellId` due to a category cooldown,
     *   and returns `false` otherwise.
     *
     * @param uint32 spellId : the ID of a [Spell]
     * @return bool hasCooldown
     */
    int HasCategoryCooldown(lua_State* L, Creature* creature)
    {
        uint32 spell = ALE::CHECKVAL<uint32>(L, 2);

        if (const SpellInfo* info = sSpellMgr->GetSpellInfo(spell))
            ALE::Push(L, info->GetCategory() && creature->HasSpellCooldown(spell));
        else
            ALE::Push(L, false);
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can cast `spellId` when mind-controlled,
     *   and returns `false` otherwise.
     *
     * @param uint32 spellId : the ID of a [Spell]
     * @return bool hasSpell
     */
    int HasSpell(lua_State* L, Creature* creature)
    {
        uint32 id = ALE::CHECKVAL<uint32>(L, 2);

        ALE::Push(L, creature->HasSpell(id));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] starts the [Quest] `questId`,
     *   and returns `false` otherwise.
     *
     * @param uint32 questId : the ID of a [Quest]
     * @return bool hasQuest
     */
    int HasQuest(lua_State* L, Creature* creature)
    {
        uint32 questId = ALE::CHECKVAL<uint32>(L, 2);

        ALE::Push(L, creature->hasQuest(questId));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] has `spellId` on cooldown,
     *   and returns `false` otherwise.
     *
     * @param uint32 spellId : the ID of a [Spell]
     * @return bool hasCooldown
     */
    int HasSpellCooldown(lua_State* L, Creature* creature)
    {
        uint32 spellId = ALE::CHECKVAL<uint32>(L, 2);

        ALE::Push(L, creature->HasSpellCooldown(spellId));
        return 1;
    }

    /**
     * Returns `true` if the [Creature] can fly,
     *   and returns `false` otherwise.
     *
     * @return bool canFly
     */
    int CanFly(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->CanFly());
        return 1;
    }

    /**
     * Returns `true` if the [Creature] is an invisible trigger,
     *   and returns `false` otherwise.
     *
     * @return bool canFly
     */
    int IsTrigger(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsTrigger());
        return 1;
    }

    /**
     * Returns true if the [Creature] is damaged enough for looting
     *
     * @return bool isDamagedEnough
     */
    int IsDamageEnoughForLootingAndReward(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->IsDamageEnoughForLootingAndReward());
        return 1;
    }

    /**
     * Returns true if the [Creature] can start attacking specified target
     *
     * Does not work on most targets
     *
     * @param [Unit] target
     * @param bool force = true : force [Creature] to attack
     */
    int CanStartAttack(lua_State* L, Creature* creature) // TODO: Implement core side
    {
        Unit* target = ALE::CHECKOBJ<Unit>(L, 2);

        ALE::Push(L, creature->CanStartAttack(target));
        return 1;
    }

    /**
     * Returns true if [Creature] has the specified loot mode
     *
     * @param uint16 lootMode
     * @return bool hasLootMode
     */
    int HasLootMode(lua_State* L, Creature* creature) // TODO: Implement LootMode features
    {
        uint16 lootMode = ALE::CHECKVAL<uint16>(L, 2);

        ALE::Push(L, creature->HasLootMode(lootMode));
        return 1;
    }

    /**
     * Returns the time it takes for this [Creature] to respawn once killed.
     *
     * This value does not usually change over a [Creature]'s lifespan,
     *   but can be modified by [Creature:SetRespawnDelay].
     *
     * @return uint32 respawnDelay : the respawn delay, in seconds
     */
    int GetRespawnDelay(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetRespawnDelay());
        return 1;
    }

    /**
     * Returns the radius the [Creature] is permitted to wander from its
     *   respawn point.
     *
     * @return float wanderRadius
     */
    int GetWanderRadius(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetWanderDistance());
        return 1;
    }

    /**
     * Returns the current waypoint path ID of the [Creature].
     *
     * @return uint32 pathId
     */
    int GetWaypointPath(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetWaypointPath());
        return 1;
    }

    /**
     * Returns the current waypoint ID of the [Creature].
     *
     * @return uint32 wpId
     */
    int GetCurrentWaypointId(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetCurrentWaypointID());
        return 1;
    }

    /**
    * Returns the spawn ID for this [Creature].
    *
    * @return uint32 spawnId
    */
    int GetSpawnId(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetSpawnId());
        return 1;
    }

    /**
     * Returns the default movement type for this [Creature].
     *
     * @return [MovementGeneratorType] defaultMovementType
     */
    int GetDefaultMovementType(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetDefaultMovementType());
        return 1;
    }

    /**
     * Returns the aggro range of the [Creature] for `target`.
     *
     * @param [Unit] target
     * @return float aggroRange
     */
    int GetAggroRange(lua_State* L, Creature* creature)
    {
        Unit* target = ALE::CHECKOBJ<Unit>(L, 2);

        ALE::Push(L, creature->GetAggroRange(target));
        return 1;
    }

    /**
     * Returns the [Group] that can loot this [Creature].
     *
     * @return [Group] lootRecipientGroup : the group or `nil`
     */
    int GetLootRecipientGroup(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetLootRecipientGroup());
        return 1;
    }

    /**
     * Returns the [Player] that can loot this [Creature].
     *
     * @return [Player] lootRecipient : the player or `nil`
     */
    int GetLootRecipient(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetLootRecipient());
        return 1;
    }

    /**
     * Returns the [Creature]'s script name.
     *
     * This is used by the core to apply C++ scripts to the Creature.
     *
     * It is not used by ALE. ALE will override AI scripts.
     *
     * @return string scriptName
     */
    int GetScriptName(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetScriptName());
        return 1;
    }

    /**
     * Returns the [Creature]'s AI name.
     *
     * This is used by the core to assign the Creature's default AI.
     *
     * If the Creature is scripted by ALE, the AI is overriden.
     *
     * @return string AIName
     */
    int GetAIName(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetAIName());
        return 1;
    }

    /**
     * Returns the [Creature]'s script ID.
     *
     * Every C++ script name is assigned a unique ID by the core.
     *   This returns the ID for this [Creature]'s script name.
     *
     * @return uint32 scriptID
     */
    int GetScriptId(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetScriptId());
        return 1;
    }

    /**
     * Returns the [Creature]'s cooldown for `spellID`.
     *
     * @param uint32 spellID
     * @return uint32 cooldown : the cooldown, in milliseconds
     */
    int GetCreatureSpellCooldownDelay(lua_State* L, Creature* creature)
    {
        uint32 spell = ALE::CHECKVAL<uint32>(L, 2);

        if (sSpellMgr->GetSpellInfo(spell))
            ALE::Push(L, creature->GetSpellCooldown(spell));
        else
            ALE::Push(L, 0);

        return 1;
    }

    /**
     * Returns the delay between when the [Creature] dies and when its body despawns.
     *
     * @return uint32 corpseDelay : the delay, in seconds
     */
    int GetCorpseDelay(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetCorpseDelay());
        return 1;
    }

    /**
     * Returns position the [Creature] returns to when evading from combat
     *   or respawning.
     *
     * @return float x
     * @return float y
     * @return float z
     * @return float o
     */
    int GetHomePosition(lua_State* L, Creature* creature)
    {
        float x, y, z, o;
        creature->GetHomePosition(x, y, z, o);

        ALE::Push(L, x);
        ALE::Push(L, y);
        ALE::Push(L, z);
        ALE::Push(L, o);
        return 4;
    }

    /**
     * Sets the position the [Creature] returns to when evading from combat
     *   or respawning.
     *
     * @param float x
     * @param float y
     * @param float z
     * @param float o
     */
    int SetHomePosition(lua_State* L, Creature* creature)
    {
        float x = ALE::CHECKVAL<float>(L, 2);
        float y = ALE::CHECKVAL<float>(L, 3);
        float z = ALE::CHECKVAL<float>(L, 4);
        float o = ALE::CHECKVAL<float>(L, 5);

        creature->SetHomePosition(x, y, z, o);
        return 0;
    }

    enum SelectAggroTarget
    {
        SELECT_TARGET_RANDOM = 0,   // Just selects a random target
        SELECT_TARGET_TOPAGGRO,     // Selects targes from top aggro to bottom
        SELECT_TARGET_BOTTOMAGGRO,  // Selects targets from bottom aggro to top
        SELECT_TARGET_NEAREST,
        SELECT_TARGET_FARTHEST
    };

    /**
    * Returns a target from the [Creature]'s threat list based on the
    *   supplied arguments.
    *
    *     enum SelectAggroTarget
    *     {
    *         SELECT_TARGET_RANDOM = 0,  //Just selects a random target
    *         SELECT_TARGET_TOPAGGRO,    //Selects targets from top aggro to bottom
    *         SELECT_TARGET_BOTTOMAGGRO, //Selects targets from bottom aggro to top
    *         SELECT_TARGET_NEAREST,
    *         SELECT_TARGET_FARTHEST
    *     };
    *
    * For example, if you wanted to select the third-farthest [Player]
    *   within 50 yards that has the [Aura] "Corrupted Blood" (ID 24328),
    *   you could use this function like so:
    *
    *     target = creature:GetAITarget(4, true, 3, 50, 24328)
    *
    * @param [SelectAggroTarget] targetType : how the threat list should be sorted
    * @param bool playerOnly = false : if `true`, skips targets that aren't [Player]s
    * @param uint32 position = 0 : used as an offset into the threat list. If `targetType` is random, used as the number of players from top of aggro to choose from
    * @param float distance = 0.0 : if positive, the maximum distance for the target. If negative, the minimum distance
    * @param int32 aura = 0 : if positive, the target must have this [Aura]. If negative, the the target must not have this Aura
    * @return [Unit] target : the target, or `nil`
    */
    int GetAITarget(lua_State* L, Creature* creature)
    {
        uint32 targetType = ALE::CHECKVAL<uint32>(L, 2);
        bool playerOnly = ALE::CHECKVAL<bool>(L, 3, false);
        uint32 position = ALE::CHECKVAL<uint32>(L, 4, 0);
        float dist = ALE::CHECKVAL<float>(L, 5, 0.0f);
        int32 aura = ALE::CHECKVAL<int32>(L, 6, 0);

        ThreatManager const& threatMgr = creature->GetThreatMgr();

        if (threatMgr.IsThreatListEmpty())
            return 1;
        if (position >= threatMgr.GetThreatListSize())
            return 1;

        std::list<Unit*> targetList;

        for (ThreatReference const* ref : threatMgr.GetSortedThreatList())
        {
            Unit* target = ref->GetVictim();

            if (!target)
                continue;
            if (playerOnly && target->GetTypeId() != TYPEID_PLAYER)
                continue;
            if (aura > 0 && !target->HasAura(aura))
                continue;
            else if (aura < 0 && target->HasAura(-aura))
                continue;
            if (dist > 0.0f && !creature->IsWithinDist(target, dist))
                continue;
            else if (dist < 0.0f && creature->IsWithinDist(target, -dist))
                continue;
            targetList.push_back(target);
        }

        if (targetList.empty())
            return 1;
        if (position >= targetList.size())
            return 1;

        if (targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
            targetList.sort(ALEUtil::ObjectDistanceOrderPred(creature));

        switch (targetType)
        {
            case SELECT_TARGET_NEAREST:
            case SELECT_TARGET_TOPAGGRO:
                {
                    std::list<Unit*>::const_iterator itr = targetList.begin();
                    if (position)
                        std::advance(itr, position);
                    ALE::Push(L, *itr);
                }
                break;
            case SELECT_TARGET_FARTHEST:
            case SELECT_TARGET_BOTTOMAGGRO:
                {
                    std::list<Unit*>::reverse_iterator ritr = targetList.rbegin();
                    if (position)
                        std::advance(ritr, position);
                    ALE::Push(L, *ritr);
                }
                break;
            case SELECT_TARGET_RANDOM:
                {
                    std::list<Unit*>::const_iterator itr = targetList.begin();
                    if (position)
                        std::advance(itr, urand(0, position));
                    else
                        std::advance(itr, urand(0, targetList.size() - 1));
                    ALE::Push(L, *itr);
                }
                break;
            default:
                luaL_argerror(L, 2, "SelectAggroTarget expected");
                break;
        }

        return 1;
    }

    /**
     * Returns all [Unit]s in the [Creature]'s threat list.
     *
     * @return table targets
     */
    int GetAITargets(lua_State* L, Creature* creature)
    {
        ThreatManager const& threatMgr = creature->GetThreatMgr();

        lua_createtable(L, threatMgr.GetThreatListSize(), 0);
        int tbl = lua_gettop(L);
        uint32 i = 0;
        for (ThreatReference const* ref : threatMgr.GetSortedThreatList())
        {
            ALE::Push(L, ref->GetVictim());
            lua_rawseti(L, tbl, ++i);
        }

        lua_settop(L, tbl);
        return 1;
    }

    /**
     * Returns the number of [Unit]s in this [Creature]'s threat list.
     *
     * @return int targetsCount
     */
    int GetAITargetsCount(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetThreatMgr().GetThreatListSize());
        return 1;
    }

    /**
     * Returns the [Creature]'s NPC flags.
     *
     * These are used to control whether the NPC is a vendor, can repair items,
     *   can give quests, etc.
     *
     * @return [NPCFlags] npcFlags
     */
    int GetNPCFlags(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetUInt32Value(UNIT_NPC_FLAGS));
        return 1;
    }

    /**
     * Returns the [Creature]'s Unit flags.
     *
     * These are used to control whether the NPC is attackable or not, among other things.
     *
     * @return [UnitFlags] unitFlags
     */
    int GetUnitFlags(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetUInt32Value(UNIT_FIELD_FLAGS));
        return 1;
    }

    /**
     * Returns the [Creature]'s Unit flags 2.
     *
     * @return [UnitFlags2] unitFlags2
     */
    int GetUnitFlagsTwo(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetUInt32Value(UNIT_FIELD_FLAGS_2));
        return 1;
    }

    /**
     * Returns the [Creature]'s Extra flags.
     *
     * These are used to control whether the NPC is a civilian, uses pathfinding,
     *   if it's a guard, etc.
     *
     * @return [ExtraFlags] extraFlags
     */
    int GetExtraFlags(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetCreatureTemplate()->flags_extra);
        return 1;
    }

    /**
     * Returns the [Creature]'s rank.
     *
     * @return [Rank] rank
     */
    int GetRank(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetCreatureTemplate()->rank);
        return 1;
    }

    /**
     * Returns the [Creature]'s shield block value.
     *
     * @return uint32 shieldBlockValue
     */
    int GetShieldBlockValue(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->GetShieldBlockValue());
        return 1;
    }

    /**
     * Returns the loot mode flags for the specified [Creature].
     *
     * @param [Creature] creature : the creature whose loot mode to get
     * @return uint16 lootMode : the loot mode bitmask of the creature
     */
    int GetLootMode(lua_State* L, Creature* creature) // TODO: Implement LootMode features
    {
        ALE::Push(L, creature->GetLootMode());
        return 1;
    }

    /**
     * Returns the [Creature]'s current ReactState.
     *
     * <pre>
     * enum ReactState
     * {
     *     REACT_PASSIVE       = 0,
     *     REACT_DEFENSIVE     = 1,
     *     REACT_AGGRESSIVE    = 2
     * };
     * </pre>
     *
     * @return [ReactState] state
     */
    int GetReactState(lua_State* L, Creature* creature)
    {
        ReactStates state = creature->GetReactState();
        lua_pushinteger(L, (int)state);
        return 1;
    }

    /**
     * Sets the [Creature]'s NPC flags to `flags`.
     *
     * @param [NPCFlags] flags
     */
    int SetNPCFlags(lua_State* L, Creature* creature)
    {
        uint32 flags = ALE::CHECKVAL<uint32>(L, 2);

        creature->SetUInt32Value(UNIT_NPC_FLAGS, flags);
        return 0;
    }
    
    /**
     * Sets the [Creature]'s Unit flags to `flags`.
     *
     * @param [UnitFlags] flags
     */
    int SetUnitFlags(lua_State* L, Creature* creature)
    {
        uint32 flags = ALE::CHECKVAL<uint32>(L, 2);
        creature->SetUInt32Value(UNIT_FIELD_FLAGS, flags);
        return 0;
    }

    /**
     * Sets the [Creature]'s Unit flags2 to `flags`.
     *
     * @param [UnitFlags2] flags
     */
    int SetUnitFlagsTwo(lua_State* L, Creature* creature)
    {
        uint32 flags = ALE::CHECKVAL<uint32>(L, 2);
        creature->SetUInt32Value(UNIT_FIELD_FLAGS_2, flags);
        return 0;
    }

    /**
     * Sets the [Creature]'s current ReactState.
     *
     * <pre>
     * enum ReactState
     * {
     *     REACT_PASSIVE       = 0,
     *     REACT_DEFENSIVE     = 1,
     *     REACT_AGGRESSIVE    = 2
     * };
     * </pre>
     *
     * @param [ReactState] state
     */
    int SetReactState(lua_State* L, Creature* creature)
    {
        uint32 state = ALE::CHECKVAL<uint32>(L, 2);

        creature->SetReactState((ReactStates)state);
        return 0;
    }

    /**
     * Makes the [Creature] able to fly if enabled.
     *
     * @param bool disable
     */
    int SetDisableGravity(lua_State* L, Creature* creature)
    {
        bool disable = ALE::CHECKVAL<bool>(L, 2);

        creature->SetDisableGravity(disable);
        return 0;
    }

    /**
     * Sets the loot mode flags for the specified [Creature].
     *
     * @param [Creature] creature : the creature whose loot mode to set
     * @param uint16 lootMode : the loot mode bitmask to apply
     */
    int SetLootMode(lua_State* L, Creature* creature) // TODO: Implement LootMode features
    {
        uint16 lootMode = ALE::CHECKVAL<uint16>(L, 2);

        creature->SetLootMode(lootMode);
        return 0;
    }

    /**
     * Sets the [Creature]'s death state to `deathState`.
     *
     * @param [DeathState] deathState
     */
    int SetDeathState(lua_State* L, Creature* creature)
    {
        int32 state = ALE::CHECKVAL<int32>(L, 2);

        creature->setDeathState((DeathState)state);
        return 0;
    }

    /**
     * Sets whether the [Creature] is currently walking or running.
     *
     * @param bool enable = true : `true` to enable walking, `false` for running
     */
    int SetWalk(lua_State* L, Creature* creature)           // TODO: Move same to Player ?
    {
        bool enable = ALE::CHECKVAL<bool>(L, 2, true);

        creature->SetWalk(enable);
        return 0;
    }

    /**
     * Equips given [Item]s to the [Unit]. Using 0 removes the equipped [Item]
     *
     * @param uint32 main_hand : main hand [Item]'s entry
     * @param uint32 off_hand : off hand [Item]'s entry
     * @param uint32 ranged : ranged [Item]'s entry
     */
    int SetEquipmentSlots(lua_State* L, Creature* creature)
    {
        uint32 main_hand = ALE::CHECKVAL<uint32>(L, 2);
        uint32 off_hand = ALE::CHECKVAL<uint32>(L, 3);
        uint32 ranged = ALE::CHECKVAL<uint32>(L, 4);

        creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, main_hand);
        creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, off_hand);
        creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, ranged);

        return 0;
    }

    /**
     * Sets whether the [Creature] can be aggroed.
     *
     * @param bool allow = true : `true` to allow aggro, `false` to disable aggro
     */
    int SetAggroEnabled(lua_State* L, Creature* creature)
    {
        bool allow = ALE::CHECKVAL<bool>(L, 2, true);

        if (allow)
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        else
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);

        return 0;
    }

    /**
     * Sets whether the [Creature] gives reputation or not.
     *
     * @param bool disable = true : `true` to disable reputation, `false` to enable
     */
    int SetDisableReputationGain(lua_State* L, Creature* creature)
    {
        bool disable = ALE::CHECKVAL<bool>(L, 2, true);

        creature->SetReputationRewardDisabled(disable);
        return 0;
    }

    /**
     * Sets the [Creature] as in combat with all [Player]s in the dungeon instance.
     *
     * This is used by raid bosses to prevent Players from using out-of-combat
     *   actions once the encounter has begun.
     */
    int SetInCombatWithZone(lua_State* /*L*/, Creature* creature)
    {
        if (creature->IsAIEnabled)
            creature->AI()->DoZoneInCombat();

        return 0;
    }

    /**
     * Sets the distance the [Creature] can wander from it's spawn point.
     *
     * @param float distance
     */
    int SetWanderRadius(lua_State* L, Creature* creature)
    {
        float dist = ALE::CHECKVAL<float>(L, 2);

        creature->SetWanderDistance(dist);

        return 0;
    }

    /**
     * Sets the time it takes for the [Creature] to respawn when killed.
     *
     * @param uint32 delay : the delay, in seconds
     */
    int SetRespawnDelay(lua_State* L, Creature* creature)
    {
        uint32 delay = ALE::CHECKVAL<uint32>(L, 2);

        creature->SetRespawnDelay(delay);
        return 0;
    }

    /**
     * Sets the default movement type of the [Creature].
     *
     * @param [MovementGeneratorType] type
     */
    int SetDefaultMovementType(lua_State* L, Creature* creature)
    {
        int32 type = ALE::CHECKVAL<int32>(L, 2);

        creature->SetDefaultMovementType((MovementGeneratorType)type);
        return 0;
    }

    /**
     * Sets whether the [Creature] can search for assistance at low health or not.
     *
     * @param bool enable = true : `true` to disable searching, `false` to allow
     */
    int SetNoSearchAssistance(lua_State* L, Creature* creature)
    {
        bool val = ALE::CHECKVAL<bool>(L, 2, true);

        creature->SetNoSearchAssistance(val);
        return 0;
    }

    /**
     * Sets whether the [Creature] can call nearby enemies for help in combat or not.
     *
     * @param bool enable = true : `true` to disable calling for help, `false` to enable
     */
    int SetNoCallAssistance(lua_State* L, Creature* creature)
    {
        bool val = ALE::CHECKVAL<bool>(L, 2, true);

        creature->SetNoCallAssistance(val);
        return 0;
    }

    /**
     * Sets whether the creature is hovering / levitating or not.
     *
     * @param bool enable = true : `true` to enable hovering, `false` to disable
     */
    int SetHover(lua_State* L, Creature* creature)
    {
        bool enable = ALE::CHECKVAL<bool>(L, 2, true);

        creature->SetHover(enable);

        return 0;
    }

    /**
     * Despawn this [Creature].
     *
     * @param uint32 delay = 0 : dely to despawn in milliseconds
     */
    int DespawnOrUnsummon(lua_State* L, Creature* creature)
    {
        uint32 msTimeToDespawn = ALE::CHECKVAL<uint32>(L, 2, 0);
        creature->DespawnOrUnsummon(Milliseconds(msTimeToDespawn));

        return 0;
    }

    /**
     * Respawn this [Creature].
     */
    int Respawn(lua_State* /*L*/, Creature* creature)
    {
        creature->Respawn();
        return 0;
    }

    /**
     * Remove this [Creature]'s corpse.
     */
    int RemoveCorpse(lua_State* /*L*/, Creature* creature)
    {
        creature->RemoveCorpse();
        return 0;
    }

    /**
     * Handles this [Creature]'s corpse state after all loot is removed.
     */
    int AllLootRemovedFromCorpse(lua_State* /*L*/, Creature* creature)
    {
        creature->AllLootRemovedFromCorpse();
        return 0;
    }

    /**
     * Sets the time it takes for the [Creature]'s corpse to despawn when killed.
     *
     * @param uint32 delay : the delay, in seconds
     */
    int SetCorpseDelay(lua_State* L, Creature* creature)
    {
        uint32 delay = ALE::CHECKVAL<uint32>(L, 2);
        creature->SetCorpseDelay(delay);
        return 0;
    }

    /**
     * Make the [Creature] start following its waypoint path.
     */
    int MoveWaypoint(lua_State* /*L*/, Creature* creature)
    {
        creature->GetMotionMaster()->MoveWaypoint(creature->GetWaypointPath(), true);

        return 0;
    }

    /**
     * Make the [Creature] call for assistance in combat from other nearby [Creature]s.
     */
    int CallAssistance(lua_State* /*L*/, Creature* creature)
    {
        creature->CallAssistance();
        return 0;
    }

    /**
     * Make the [Creature] call for help in combat from friendly [Creature]s within `radius`.
     *
     * @param float radius
     */
    int CallForHelp(lua_State* L, Creature* creature)
    {
        float radius = ALE::CHECKVAL<float>(L, 2);

        creature->CallForHelp(radius);
        return 0;
    }

    /**
     * Make the [Creature] flee combat to get assistance from a nearby friendly [Creature].
     */
    int FleeToGetAssistance(lua_State* /*L*/, Creature* creature)
    {
        creature->DoFleeToGetAssistance();
        return 0;
    }

    /**
     * Make the [Creature] attack `target`.
     *
     * @param [Unit] target
     */
    int AttackStart(lua_State* L, Creature* creature)
    {
        Unit* target = ALE::CHECKOBJ<Unit>(L, 2);

        creature->AI()->AttackStart(target);
        return 0;
    }

    /**
     * Save the [Creature] in the database.
     */
    int SaveToDB(lua_State* /*L*/, Creature* creature)
    {
        creature->SaveToDB();
        return 0;
    }

    /**
     * Make the [Creature] try to find a new target.
     *
     * This should be called every update cycle for the Creature's AI.
     */
    int SelectVictim(lua_State* L, Creature* creature)
    {
        ALE::Push(L, creature->SelectVictim());
        return 1;
    }

    /**
     * Transform the [Creature] into another Creature.
     *
     * @param uint32 entry : the Creature ID to transform into
     * @param uint32 dataGUIDLow = 0 : use this Creature's model and equipment instead of the defaults
     */
    int UpdateEntry(lua_State* L, Creature* creature)
    {
        uint32 entry = ALE::CHECKVAL<uint32>(L, 2);
        uint32 dataGuidLow = ALE::CHECKVAL<uint32>(L, 3, 0);

        creature->UpdateEntry(entry, dataGuidLow ? eObjectMgr->GetCreatureData(dataGuidLow) : NULL);
        return 0;
    }

    /**
     * Resets [Creature]'s loot mode to default
     */
    int ResetLootMode(lua_State* /*L*/, Creature* creature) // TODO: Implement LootMode features
    {
        creature->ResetLootMode();
        return 0;
    }

    /**
     * Removes specified loot mode from [Creature]
     *
     * @param uint16 lootMode
     */
    int RemoveLootMode(lua_State* L, Creature* creature) // TODO: Implement LootMode features
    {
        uint16 lootMode = ALE::CHECKVAL<uint16>(L, 2);

        creature->RemoveLootMode(lootMode);
        return 0;
    }

    /**
     * Adds a loot mode to the [Creature]
     *
     * @param uint16 lootMode
     */
    int AddLootMode(lua_State* L, Creature* creature) // TODO: Implement LootMode features
    {
        uint16 lootMode = ALE::CHECKVAL<uint16>(L, 2);

        creature->AddLootMode(lootMode);
        return 0;
    }

    /**
     * Returns the [Creature]'s creature family ID (enumerated in CreatureFamily.dbc).
     *
     * <pre>
     * enum CreatureFamily
     * {
     *     CREATURE_FAMILY_NONE                = 0,    // TrinityCore only
     *     CREATURE_FAMILY_WOLF                = 1,
     *     CREATURE_FAMILY_CAT                 = 2,
     *     CREATURE_FAMILY_SPIDER              = 3,
     *     CREATURE_FAMILY_BEAR                = 4,
     *     CREATURE_FAMILY_BOAR                = 5,
     *     CREATURE_FAMILY_CROCOLISK           = 6,
     *     CREATURE_FAMILY_CARRION_BIRD        = 7,
     *     CREATURE_FAMILY_CRAB                = 8,
     *     CREATURE_FAMILY_GORILLA             = 9,
     *     CREATURE_FAMILY_HORSE_CUSTOM        = 10,   // Does not exist in DBC but used for horse like beasts in DB
     *     CREATURE_FAMILY_RAPTOR              = 11,
     *     CREATURE_FAMILY_TALLSTRIDER         = 12,
     *     CREATURE_FAMILY_FELHUNTER           = 15,
     *     CREATURE_FAMILY_VOIDWALKER          = 16,
     *     CREATURE_FAMILY_SUCCUBUS            = 17,
     *     CREATURE_FAMILY_DOOMGUARD           = 19,
     *     CREATURE_FAMILY_SCORPID             = 20,
     *     CREATURE_FAMILY_TURTLE              = 21,
     *     CREATURE_FAMILY_IMP                 = 23,
     *     CREATURE_FAMILY_BAT                 = 24,
     *     CREATURE_FAMILY_HYENA               = 25,
     *     CREATURE_FAMILY_BIRD_OF_PREY        = 26,   // Named CREATURE_FAMILY_OWL in Mangos
     *     CREATURE_FAMILY_WIND_SERPENT        = 27,
     *     CREATURE_FAMILY_REMOTE_CONTROL      = 28,
     *     CREATURE_FAMILY_FELGUARD            = 29,   // This and below is TBC+
     *     CREATURE_FAMILY_DRAGONHAWK          = 30,
     *     CREATURE_FAMILY_RAVAGER             = 31,
     *     CREATURE_FAMILY_WARP_STALKER        = 32,
     *     CREATURE_FAMILY_SPOREBAT            = 33,
     *     CREATURE_FAMILY_NETHER_RAY          = 34,
     *     CREATURE_FAMILY_SERPENT             = 35,
     *     CREATURE_FAMILY_SEA_LION            = 36,   // TBC only
     *     CREATURE_FAMILY_MOTH                = 37,   // This and below is WotLK+
     *     CREATURE_FAMILY_CHIMAERA            = 38,
     *     CREATURE_FAMILY_DEVILSAUR           = 39,
     *     CREATURE_FAMILY_GHOUL               = 40,
     *     CREATURE_FAMILY_SILITHID            = 41,
     *     CREATURE_FAMILY_WORM                = 42,
     *     CREATURE_FAMILY_RHINO               = 43,
     *     CREATURE_FAMILY_WASP                = 44,
     *     CREATURE_FAMILY_CORE_HOUND          = 45,
     *     CREATURE_FAMILY_SPIRIT_BEAST        = 46
     * };
     * </pre>
     *
     * @return [CreatureFamily] creatureFamily
     */
    int GetCreatureFamily(lua_State* L, Creature* creature)
    {
        uint32 entry = creature->GetEntry();

        CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(entry);
        if (cInfo)
            ALE::Push(L, cInfo->family);

        return 1;
    }

    static uint32 CountUnlootedEntriesForIndex(
        QuestItemMap const& map,
        uint8 index)
    {
        uint32 count = 0;

        for (QuestItemMap::const_iterator itr = map.begin();
             itr != map.end(); ++itr)
        {
            if (!itr->second)
                continue;

            for (QuestItem const& entry : *itr->second)
            {
                if (entry.index == index && !entry.is_looted)
                    ++count;
            }
        }

        return count;
    }

    static uint32 CountMovedUnlootedItems(
        Loot const& loot,
        std::vector<bool> const& moveRegularItems)
    {
        uint32 count = 0;

        QuestItemMap const& ffaMap = loot.GetPlayerFFAItems();
        QuestItemMap const& conditionalMap =
            loot.GetPlayerNonQuestNonFFAConditionalItems();
        QuestItemMap const& questMap = loot.GetPlayerQuestItems();

        for (std::size_t i = 0;
             i < loot.items.size() && i < moveRegularItems.size(); ++i)
        {
            if (!moveRegularItems[i])
                continue;

            LootItem const& item = loot.items[i];
            if (item.is_looted)
                continue;

            if (item.freeforall)
            {
                count += CountUnlootedEntriesForIndex(ffaMap, static_cast<uint8>(i));
            }
            else if (!item.conditions.empty())
            {
                if (CountUnlootedEntriesForIndex(conditionalMap, static_cast<uint8>(i)) > 0)
                {
                    ++count;
                }
            }
            else
            {
                ++count;
            }
        }

        for (std::size_t i = 0; i < loot.quest_items.size(); ++i)
        {
            LootItem const& item = loot.quest_items[i];
            if (item.is_looted)
                continue;

            uint32 playerEntries = CountUnlootedEntriesForIndex(
                questMap, static_cast<uint8>(i));

            if (item.freeforall)
                count += playerEntries;
            else if (playerEntries > 0)
                ++count;
        }

        return count;
    }

    static QuestItem const* FindLootViewEntry(
    QuestItemMap const& map,
    ObjectGuid playerGuid,
    uint8 index)
{
    QuestItemMap::const_iterator itr = map.find(playerGuid);
    if (itr == map.end() || !itr->second)
        return nullptr;

    for (QuestItem const& entry : *itr->second)
    {
        if (entry.index == index)
            return &entry;
    }

    return nullptr;
}

    static bool SameLootViewForIndex(
        QuestItemMap const& left,
        uint8 leftIndex,
        QuestItemMap const& right,
        uint8 rightIndex)
    {
        for (QuestItemMap::const_iterator itr = left.begin();
            itr != left.end(); ++itr)
        {
            QuestItem const* leftEntry =
                FindLootViewEntry(left, itr->first, leftIndex);

            QuestItem const* rightEntry =
                FindLootViewEntry(right, itr->first, rightIndex);

            if ((leftEntry == nullptr) != (rightEntry == nullptr))
                return false;

            if (leftEntry &&
                leftEntry->is_looted != rightEntry->is_looted)
            {
                return false;
            }
        }

        for (QuestItemMap::const_iterator itr = right.begin();
            itr != right.end(); ++itr)
        {
            QuestItem const* leftEntry =
                FindLootViewEntry(left, itr->first, leftIndex);

            QuestItem const* rightEntry =
                FindLootViewEntry(right, itr->first, rightIndex);

            if ((leftEntry == nullptr) != (rightEntry == nullptr))
                return false;
        }

        return true;
    }

    static bool SameLootItemMetadata(
        LootItem const& left,
        LootItem const& right)
    {
        return left.itemid == right.itemid &&
            left.randomSuffix == right.randomSuffix &&
            left.randomPropertyId == right.randomPropertyId &&
            left.conditions == right.conditions &&
            left.allowedGUIDs == right.allowedGUIDs &&
            left.rollWinnerGUID == right.rollWinnerGUID &&
            left.is_looted == right.is_looted &&
            left.is_blocked == right.is_blocked &&
            left.freeforall == right.freeforall &&
            left.is_underthreshold == right.is_underthreshold &&
            left.is_counted == right.is_counted &&
            left.needs_quest == right.needs_quest &&
            left.follow_loot_rules == right.follow_loot_rules &&
            left.groupid == right.groupid;
    }

    static bool CanStackRegularLootItems(
        LootItem const& destination,
        LootItem const& source,
        Group* group,
        LootMethod lootMethod)
    {
        if (!SameLootItemMetadata(destination, source))
            return false;

        if (uint32(destination.count) + uint32(source.count) > 255u)
            return false;

        ItemTemplate const* itemTemplate =
            sObjectMgr->GetItemTemplate(source.itemid);

        if (!itemTemplate || itemTemplate->MaxCount > 0)
            return false;

        if (group && !source.freeforall && !source.conditions.empty())
        {
            return false;
        }

        bool usesGroupAssignment = group &&
            (lootMethod == GROUP_LOOT ||
            lootMethod == NEED_BEFORE_GREED ||
            lootMethod == MASTER_LOOT);

        // Separate above-threshold entries must retain separate rolls
        // or master-loot assignments.
        if (usesGroupAssignment &&
            !source.freeforall &&
            !source.is_underthreshold)
        {
            return false;
        }

        return true;
    }

    static bool CanStackQuestLootItems(
        LootItem const& destination,
        LootItem const& source,
        Group* group)
    {
        if (!SameLootItemMetadata(destination, source))
            return false;

        if (uint32(destination.count) + uint32(source.count) > 255u)
            return false;

        ItemTemplate const* itemTemplate =
            sObjectMgr->GetItemTemplate(source.itemid);

        if (!itemTemplate || itemTemplate->MaxCount > 0)
            return false;

        // In groups, only per-player/FFA quest loot is safe to stack.
        if (group && !source.freeforall)
            return false;

        return true;
    }

    static void RemapLootViewMap(
        QuestItemMap& map,
        std::vector<int32> const& indexMap)
    {
        for (QuestItemMap::iterator itr = map.begin();
            itr != map.end();)
        {
            QuestItemList* list = itr->second;
            if (!list)
            {
                itr = map.erase(itr);
                continue;
            }

            QuestItemList remapped;

            for (QuestItem const& entry : *list)
            {
                if (entry.index >= indexMap.size())
                    continue;

                int32 newIndex = indexMap[entry.index];
                if (newIndex < 0)
                    continue;

                bool found = false;

                for (QuestItem& existing : remapped)
                {
                    if (existing.index ==
                        static_cast<uint8>(newIndex))
                    {
                        // Compatible slots have matching state. AND is
                        // conservative if inconsistent input is encountered.
                        existing.is_looted =
                            existing.is_looted && entry.is_looted;
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    QuestItem copiedEntry = entry;
                    copiedEntry.index = static_cast<uint8>(newIndex);
                    remapped.push_back(copiedEntry);
                }
            }

            if (remapped.empty())
            {
                delete list;
                itr = map.erase(itr);
            }
            else
            {
                *list = std::move(remapped);
                ++itr;
            }
        }
    }

    static void CoalesceCompatibleLoot(
        Loot& loot,
        Group* group,
        LootMethod lootMethod)
    {
        QuestItemMap& ffaMap =
            const_cast<QuestItemMap&>(loot.GetPlayerFFAItems());

        QuestItemMap& conditionalMap =
            const_cast<QuestItemMap&>(loot.GetPlayerNonQuestNonFFAConditionalItems());

        QuestItemMap& questMap =
            const_cast<QuestItemMap&>(loot.GetPlayerQuestItems());

        // Regular items
        std::vector<LootItem> oldItems = std::move(loot.items);
        std::vector<int32> itemIndexMap(oldItems.size(), -1);
        std::vector<uint8> representativeIndex;

        loot.items.clear();
        loot.items.reserve(oldItems.size());

        for (std::size_t oldIndex = 0;
            oldIndex < oldItems.size(); ++oldIndex)
        {
            LootItem const& sourceItem = oldItems[oldIndex];
            int32 destinationIndex = -1;

            for (std::size_t currentIndex = 0;
                currentIndex < loot.items.size(); ++currentIndex)
            {
                if (!CanStackRegularLootItems(
                        loot.items[currentIndex],
                        sourceItem,
                        group,
                        lootMethod))
                {
                    continue;
                }

                uint8 representative =
                    representativeIndex[currentIndex];

                bool sameView = true;

                if (sourceItem.freeforall)
                {
                    sameView = SameLootViewForIndex(
                        ffaMap,
                        static_cast<uint8>(oldIndex),
                        ffaMap,
                        representative);
                }
                else if (!sourceItem.conditions.empty())
                {
                    sameView = SameLootViewForIndex(
                        conditionalMap,
                        static_cast<uint8>(oldIndex),
                        conditionalMap,
                        representative);
                }

                if (!sameView)
                    continue;

                destinationIndex = static_cast<int32>(currentIndex);
                break;
            }

            if (destinationIndex >= 0)
            {
                LootItem& destination = loot.items[destinationIndex];

                destination.count = static_cast<uint8>(
                    uint32(destination.count) + uint32(sourceItem.count));

                itemIndexMap[oldIndex] = destinationIndex;
            }
            else
            {
                LootItem copiedItem = sourceItem;
                copiedItem.itemIndex = static_cast<uint32>(loot.items.size());

                itemIndexMap[oldIndex] = static_cast<int32>(loot.items.size());

                representativeIndex.push_back(static_cast<uint8>(oldIndex));

                loot.items.push_back(copiedItem);
            }
        }

        RemapLootViewMap(ffaMap, itemIndexMap);
        RemapLootViewMap(conditionalMap, itemIndexMap);

        // Quest items
        std::vector<LootItem> oldQuestItems = std::move(loot.quest_items);

        std::vector<int32> questIndexMap(oldQuestItems.size(), -1);

        std::vector<uint8> questRepresentativeIndex;

        loot.quest_items.clear();
        loot.quest_items.reserve(oldQuestItems.size());

        for (std::size_t oldIndex = 0;
            oldIndex < oldQuestItems.size(); ++oldIndex)
        {
            LootItem const& sourceItem = oldQuestItems[oldIndex];

            int32 destinationIndex = -1;

            for (std::size_t currentIndex = 0;
                currentIndex < loot.quest_items.size();
                ++currentIndex)
            {
                if (!CanStackQuestLootItems(
                        loot.quest_items[currentIndex],
                        sourceItem,
                        group))
                {
                    continue;
                }

                uint8 representative =
                    questRepresentativeIndex[currentIndex];

                if (!SameLootViewForIndex(
                        questMap,
                        static_cast<uint8>(oldIndex),
                        questMap,
                        representative))
                {
                    continue;
                }

                destinationIndex = static_cast<int32>(currentIndex);
                break;
            }

            if (destinationIndex >= 0)
            {
                LootItem& destination = loot.quest_items[destinationIndex];

                destination.count = static_cast<uint8>(
                    uint32(destination.count) + uint32(sourceItem.count));

                questIndexMap[oldIndex] = destinationIndex;
            }
            else
            {
                LootItem copiedItem = sourceItem;
                copiedItem.itemIndex = static_cast<uint32>(loot.quest_items.size());

                questIndexMap[oldIndex] = static_cast<int32>(loot.quest_items.size());

                questRepresentativeIndex.push_back(static_cast<uint8>(oldIndex));

                loot.quest_items.push_back(copiedItem);
            }
        }

        RemapLootViewMap(questMap, questIndexMap);

        std::vector<bool> allRegularItems(loot.items.size(), true);

        loot.unlootedCount = static_cast<uint8>(
            CountMovedUnlootedItems(loot, allRegularItems));
    }

    static bool CanAppendQuestViews(
        QuestItemMap const& destination,
        QuestItemMap const& source,
        std::size_t regularSlots)
    {
        for (QuestItemMap::const_iterator itr = destination.begin();
             itr != destination.end(); ++itr)
        {
            std::size_t questSlots = itr->second ? itr->second->size() : 0;

            QuestItemMap::const_iterator sourceItr =
                source.find(itr->first);

            if (sourceItr != source.end() && sourceItr->second)
                questSlots += sourceItr->second->size();

            if (regularSlots + questSlots > MAX_NR_LOOT_ITEMS)
                return false;
        }

        for (QuestItemMap::const_iterator itr = source.begin();
             itr != source.end(); ++itr)
        {
            if (destination.find(itr->first) != destination.end())
                continue;

            std::size_t questSlots = itr->second ? itr->second->size() : 0;

            if (regularSlots + questSlots > MAX_NR_LOOT_ITEMS)
                return false;
        }

        return true;
    }

    static void SplitLootViewMap(
        QuestItemMap& source,
        QuestItemMap& destination,
        std::vector<int32> const& sourceIndexByOldIndex,
        std::vector<int32> const& destinationIndexByOldIndex)
    {
        for (QuestItemMap::iterator itr = source.begin();
             itr != source.end();)
        {
            QuestItemList* sourceList = itr->second;
            if (!sourceList)
            {
                itr = source.erase(itr);
                continue;
            }

            QuestItemList retained;

            for (QuestItem const& sourceEntry : *sourceList)
            {
                std::size_t oldIndex = sourceEntry.index;
                if (oldIndex >= sourceIndexByOldIndex.size() ||
                    oldIndex >= destinationIndexByOldIndex.size())
                {
                    continue;
                }

                int32 destinationIndex =
                    destinationIndexByOldIndex[oldIndex];

                if (destinationIndex >= 0)
                {
                    QuestItemList*& destinationList = destination[itr->first];

                    if (!destinationList)
                        destinationList = new QuestItemList();

                    QuestItem copiedEntry = sourceEntry;
                    copiedEntry.index = static_cast<uint8>(destinationIndex);
                    destinationList->push_back(copiedEntry);
                    continue;
                }

                int32 retainedIndex =
                    sourceIndexByOldIndex[oldIndex];

                if (retainedIndex >= 0)
                {
                    QuestItem copiedEntry = sourceEntry;
                    copiedEntry.index = static_cast<uint8>(retainedIndex);
                    retained.push_back(copiedEntry);
                }
            }

            if (retained.empty())
            {
                delete sourceList;
                itr = source.erase(itr);
            }
            else
            {
                *sourceList = std::move(retained);
                ++itr;
            }
        }
    }

    static bool ShouldUseOwnerSpecificLootView(
        LootItem const& item,
        Group* sourceGroup,
        LootMethod lootMethod,
        ObjectGuid sourceRoundRobinPlayer,
        ObjectGuid anchorRoundRobinPlayer)
    {
        if (item.is_looted ||
            item.freeforall ||
            !item.conditions.empty() ||
            !sourceGroup ||
            lootMethod == FREE_FOR_ALL)
        {
            return false;
        }

        if (!sourceRoundRobinPlayer ||
            sourceRoundRobinPlayer == anchorRoundRobinPlayer)
        {
            return false;
        }

        if (lootMethod == ROUND_ROBIN)
            return true;

        bool usesRoundRobinForLowQuality =
            lootMethod == GROUP_LOOT ||
            lootMethod == NEED_BEFORE_GREED ||
            lootMethod == MASTER_LOOT;

        return usesRoundRobinForLowQuality &&
            item.is_underthreshold;
    }

    static void MakeRegularLootOwnerSpecific(
        Loot& loot,
        std::size_t itemIndex,
        ObjectGuid ownerGuid)
    {
        if (!ownerGuid || itemIndex >= loot.items.size())
            return;

        LootItem& item = loot.items[itemIndex];
        item.freeforall = true;

        QuestItemMap& ffaMap =
            const_cast<QuestItemMap&>(
                loot.GetPlayerFFAItems());

        QuestItemList*& ownerList = ffaMap[ownerGuid];

        if (!ownerList)
            ownerList = new QuestItemList();

        for (QuestItem const& entry : *ownerList)
        {
            if (entry.index == itemIndex)
                return;
        }

        ownerList->push_back(
            QuestItem(static_cast<uint8>(itemIndex)));
    }

    static bool ShouldMoveRegularItem(
        LootItem const& item,
        Group* sourceGroup,
        LootMethod lootMethod,
        ObjectGuid sourceRoundRobinPlayer,
        ObjectGuid anchorRoundRobinPlayer)
    {
        if (item.is_looted)
            return false;

        // Per-player maps preserve access for these items.
        if (item.freeforall || !item.conditions.empty())
            return true;

        if (!sourceGroup || lootMethod == FREE_FOR_ALL)
            return true;

        bool sameRoundRobinOwner =
            sourceRoundRobinPlayer == anchorRoundRobinPlayer;

        if (lootMethod == ROUND_ROBIN)
            return sameRoundRobinOwner;

        return !item.is_underthreshold || sameRoundRobinOwner;
    }

    /**
     * Selectively consolidates unopened source-creature loot into this
     * creature while preserving stock AzerothCore group-loot behavior.
     *
     * Round-robin loot from different owners is preserved through
     * owner specific per-player loot views.
     *
     * @param Creature source
     * @param Player player
     * @return bool merged
     */
    int MergeLootFrom(lua_State* L, Creature* anchor)
    {
        Creature* source = ALE::CHECKOBJ<Creature>(L, 2);
        Player* player = ALE::CHECKOBJ<Player>(L, 3);

        auto ReturnResult = [L](bool result)
        {
            ALE::Push(L, result);
            return 1;
        };

        if (!source || !player || source == anchor)
            return ReturnResult(false);

        if (!player->IsAlive() || anchor->IsAlive() || source->IsAlive())
            return ReturnResult(false);

        if (!anchor->GetMap() ||
            anchor->GetMap() != source->GetMap() ||
            anchor->GetMap() != player->GetMap())
        {
            return ReturnResult(false);
        }

        // Prevent forged remote-loot packets from moving loot.
        if (!player->IsWithinDistInMap(anchor, INTERACTION_DISTANCE) ||
            !player->IsWithinDistInMap(source, 50.0f))
        {
            return ReturnResult(false);
        }

        if (!anchor->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE) ||
            !source->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE))
        {
            return ReturnResult(false);
        }

        Group* playerGroup = player->GetGroup();
        Group* anchorGroup = anchor->GetLootRecipientGroup();
        Group* sourceGroup = source->GetLootRecipientGroup();

        if (playerGroup)
        {
            if (anchorGroup != playerGroup || sourceGroup != playerGroup)
                return ReturnResult(false);
        }
        else
        {
            if (anchorGroup || sourceGroup ||
                anchor->GetLootRecipientGUID() != player->GetGUID() ||
                source->GetLootRecipientGUID() != player->GetGUID())
            {
                return ReturnResult(false);
            }
        }

        if (!player->isAllowedToLoot(anchor))
            return ReturnResult(false);

        Loot& anchorLoot = anchor->loot;
        Loot& sourceLoot = source->loot;

        // Only untouched loot may be consolidated. Active rolls and
        // partially looted per-player containers remain unchanged.
        if (anchorLoot.loot_type != LOOT_NONE ||
            sourceLoot.loot_type != LOOT_NONE ||
            sourceLoot.isLooted())
        {
            return ReturnResult(false);
        }

        if (sourceLoot.items.empty() &&
            sourceLoot.quest_items.empty() &&
            sourceLoot.gold == 0)
        {
            return ReturnResult(false);
        }

        LootMethod lootMethod =
            sourceGroup ? sourceGroup->GetLootMethod() : FREE_FOR_ALL;

        CoalesceCompatibleLoot(anchorLoot, sourceGroup, lootMethod);
        CoalesceCompatibleLoot(sourceLoot, sourceGroup, lootMethod);

        std::vector<bool> moveRegularItems(sourceLoot.items.size(), false);

        std::size_t movedRegularCount = 0;

        for (std::size_t i = 0; i < sourceLoot.items.size(); ++i)
        {
            if (ShouldUseOwnerSpecificLootView(
                    sourceLoot.items[i],
                    sourceGroup,
                    lootMethod,
                    sourceLoot.roundRobinPlayer,
                    anchorLoot.roundRobinPlayer))
            {
                MakeRegularLootOwnerSpecific(
                    sourceLoot,
                    i,
                    sourceLoot.roundRobinPlayer);
            }

            if (ShouldMoveRegularItem(
                    sourceLoot.items[i],
                    sourceGroup,
                    lootMethod,
                    sourceLoot.roundRobinPlayer,
                    anchorLoot.roundRobinPlayer))
            {
                moveRegularItems[i] = true;
                ++movedRegularCount;
            }
        }

        bool hasMovableLoot =
            movedRegularCount > 0 ||
            !sourceLoot.quest_items.empty() ||
            sourceLoot.gold > 0;

        if (!hasMovableLoot)
            return ReturnResult(false);

        std::size_t newRegularCount =
            anchorLoot.items.size() + movedRegularCount;

        if (newRegularCount > MAX_NR_LOOT_ITEMS ||
            anchorLoot.quest_items.size() + sourceLoot.quest_items.size() > MAX_NR_QUEST_ITEMS)
        {
            return ReturnResult(false);
        }

        QuestItemMap& anchorQuestMap =
            const_cast<QuestItemMap&>(anchorLoot.GetPlayerQuestItems());

        QuestItemMap& sourceQuestMap =
            const_cast<QuestItemMap&>(sourceLoot.GetPlayerQuestItems());

        if (!CanAppendQuestViews(anchorQuestMap, sourceQuestMap, newRegularCount))
        {
            return ReturnResult(false);
        }

        uint32 movedUnlootedCount = CountMovedUnlootedItems(sourceLoot, moveRegularItems);

        if (movedUnlootedCount > sourceLoot.unlootedCount ||
            uint32(anchorLoot.unlootedCount) + movedUnlootedCount > 255u ||
            sourceLoot.gold > 0xFFFFFFFFu - anchorLoot.gold)
        {
            return ReturnResult(false);
        }

        std::vector<int32> sourceItemIndex(sourceLoot.items.size(), -1);
        std::vector<int32> anchorItemIndex(sourceLoot.items.size(), -1);

        std::vector<LootItem> retainedItems;
        retainedItems.reserve(sourceLoot.items.size() - movedRegularCount);

        for (std::size_t oldIndex = 0;
             oldIndex < sourceLoot.items.size(); ++oldIndex)
        {
            LootItem copiedItem = sourceLoot.items[oldIndex];

            if (moveRegularItems[oldIndex])
            {
                copiedItem.itemIndex =
                    static_cast<uint32>(anchorLoot.items.size());

                anchorItemIndex[oldIndex] =
                    static_cast<int32>(anchorLoot.items.size());

                anchorLoot.items.push_back(copiedItem);
            }
            else
            {
                copiedItem.itemIndex =
                    static_cast<uint32>(retainedItems.size());

                sourceItemIndex[oldIndex] =
                    static_cast<int32>(retainedItems.size());

                retainedItems.push_back(copiedItem);
            }
        }

        sourceLoot.items = std::move(retainedItems);

        std::vector<int32> sourceQuestIndex(sourceLoot.quest_items.size(), -1);
        std::vector<int32> anchorQuestIndex(sourceLoot.quest_items.size(), -1);

        for (std::size_t oldIndex = 0;
             oldIndex < sourceLoot.quest_items.size(); ++oldIndex)
        {
            LootItem copiedItem =
                sourceLoot.quest_items[oldIndex];

            copiedItem.itemIndex =
                static_cast<uint32>(anchorLoot.quest_items.size());

            anchorQuestIndex[oldIndex] =
                static_cast<int32>(anchorLoot.quest_items.size());

            anchorLoot.quest_items.push_back(copiedItem);
        }

        sourceLoot.quest_items.clear();

        QuestItemMap& anchorFFAMap =
            const_cast<QuestItemMap&>(anchorLoot.GetPlayerFFAItems());

        QuestItemMap& sourceFFAMap =
            const_cast<QuestItemMap&>(sourceLoot.GetPlayerFFAItems());

        QuestItemMap& anchorConditionalMap =
            const_cast<QuestItemMap&>(anchorLoot.GetPlayerNonQuestNonFFAConditionalItems());

        QuestItemMap& sourceConditionalMap =
            const_cast<QuestItemMap&>(sourceLoot.GetPlayerNonQuestNonFFAConditionalItems());

        SplitLootViewMap(sourceFFAMap, anchorFFAMap, sourceItemIndex, anchorItemIndex);
        SplitLootViewMap(sourceConditionalMap, anchorConditionalMap, sourceItemIndex, anchorItemIndex);
        SplitLootViewMap(sourceQuestMap, anchorQuestMap, sourceQuestIndex, anchorQuestIndex);

        anchorLoot.gold += sourceLoot.gold;
        sourceLoot.gold = 0;

        anchorLoot.unlootedCount =
            static_cast<uint8>(uint32(anchorLoot.unlootedCount) + movedUnlootedCount);

        sourceLoot.unlootedCount =
            static_cast<uint8>(uint32(sourceLoot.unlootedCount) - movedUnlootedCount);

        CoalesceCompatibleLoot(anchorLoot, sourceGroup, lootMethod);

        if (sourceLoot.isLooted())
        {
            // Required for skinning and mob engineering.
            source->AllLootRemovedFromCorpse();
            sourceLoot.clear();
            source->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
        }
        else
        {
            // Round-robin loot with a different anchor owner remains here.
            source->ForceValuesUpdateAtIndex(UNIT_DYNAMIC_FLAGS);
        }

        anchor->ForceValuesUpdateAtIndex(UNIT_DYNAMIC_FLAGS);
        return ReturnResult(true);
    }

    /**
     * Returns the [Creature]'s loot.
     *
     * @return [Loot] loot : the loot object
     */
    int GetLoot(lua_State* L, Creature* creature)
    {
        ALE::Push(L, &creature->loot);
        return 1;
    }
};
#endif
