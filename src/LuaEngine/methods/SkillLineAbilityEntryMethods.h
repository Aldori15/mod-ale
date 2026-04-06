/*
* Copyright (C) 2010 - 2025 Eluna Lua Engine <https://elunaluaengine.github.io/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef SKILLLINEABILITYENTRYMETHODS_H
#define SKILLLINEABILITYENTRYMETHODS_H

/***
 * Represents a skill line ability entry from SkillLineAbility.dbc.
 *
 * Links spells to skill lines and defines acquisition conditions.
 *
 * Inherits all methods from: none
 */
namespace LuaSkillLineAbilityEntry
{
    /**
     * Returns the ID of the [SkillLineAbilityEntry].
     *
     * @return uint32 id
     */
    int GetId(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->ID);
        return 1;
    }

    /**
     * Returns the skill line ID this ability belongs to.
     *
     * @return uint32 skillLine
     */
    int GetSkillLine(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->SkillLine);
        return 1;
    }

    /**
     * Returns the spell ID associated with this ability.
     *
     * @return uint32 spell
     */
    int GetSpell(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->Spell);
        return 1;
    }

    /**
     * Returns the race mask required to learn this ability.
     *
     * @return uint32 raceMask
     */
    int GetRaceMask(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->RaceMask);
        return 1;
    }

    /**
     * Returns the class mask required to learn this ability.
     *
     * @return uint32 classMask
     */
    int GetClassMask(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->ClassMask);
        return 1;
    }

    /**
     * Returns the minimum skill rank required to learn this ability.
     *
     * @return uint32 minSkillLineRank
     */
    int GetMinSkillLineRank(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->MinSkillLineRank);
        return 1;
    }

    /**
     * Returns the spell that supersedes this ability (next rank), or 0 if none.
     *
     * @return uint32 supercededBySpell
     */
    int GetSupercededBySpell(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->SupercededBySpell);
        return 1;
    }

    /**
     * Returns how the ability is acquired (0 = on skill up, 1 = from trainer, 2 = automatically).
     *
     * @return uint32 acquireMethod
     */
    int GetAcquireMethod(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->AcquireMethod);
        return 1;
    }

    /**
     * Returns the trivial skill rank (upper bound) — above this rank the ability gives no skill-ups.
     *
     * @return uint32 trivialSkillLineRankHigh
     */
    int GetTrivialSkillLineRankHigh(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->TrivialSkillLineRankHigh);
        return 1;
    }

    /**
     * Returns the trivial skill rank (lower bound) — below this rank the ability always gives skill-ups.
     *
     * @return uint32 trivialSkillLineRankLow
     */
    int GetTrivialSkillLineRankLow(lua_State* L, SkillLineAbilityEntry* entry)
    {
        ALE::Push(L, entry->TrivialSkillLineRankLow);
        return 1;
    }
}
#endif
