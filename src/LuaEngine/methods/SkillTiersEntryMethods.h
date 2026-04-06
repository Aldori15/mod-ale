/*
* Copyright (C) 2010 - 2025 Eluna Lua Engine <https://elunaluaengine.github.io/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef SKILLTIERSENTRY_METHODS_H
#define SKILLTIERSENTRY_METHODS_H

/***
 * Represents a skill tiers entry from SkillTiers.dbc.
 *
 * Defines the maximum skill value for each tier step (up to 16 steps).
 *
 * Inherits all methods from: none
 */
namespace LuaSkillTiersEntry
{
    /**
     * Returns the ID of the [SkillTiersEntry].
     *
     * @return uint32 id
     */
    int GetId(lua_State* L, SkillTiersEntry* entry)
    {
        ALE::Push(L, entry->ID);
        return 1;
    }

    /**
     * Returns the maximum skill value for a given tier step (1-based index, 1 to 16).
     *
     * @param uint32 step : tier step index (1 to 16)
     * @return uint32 value
     */
    int GetValue(lua_State* L, SkillTiersEntry* entry)
    {
        uint32 step = ALE::CHECKVAL<uint32>(L, 2);
        if (step < 1 || step > MAX_SKILL_STEP)
        {
            return luaL_error(L, "SkillTiersEntry:GetValue - step index out of range (1-%d)", MAX_SKILL_STEP);
        }
        ALE::Push(L, entry->Value[step - 1]);
        return 1;
    }
}
#endif
