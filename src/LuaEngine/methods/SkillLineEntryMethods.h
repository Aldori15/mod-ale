/*
* Copyright (C) 2010 - 2025 Eluna Lua Engine <https://elunaluaengine.github.io/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef SKILLLINEENTRYMETHODS_H
#define SKILLLINEENTRYMETHODS_H

/***
 * Represents a skill line entry from SkillLine.dbc.
 *
 * Provides access to skill metadata such as name, category, and icon.
 *
 * Inherits all methods from: none
 */
namespace LuaSkillLineEntry
{
    /**
     * Returns the ID of the [SkillLineEntry].
     *
     * @return uint32 id
     */
    int GetId(lua_State* L, SkillLineEntry* entry)
    {
        ALE::Push(L, entry->id);
        return 1;
    }

    /**
     * Returns the category ID of the [SkillLineEntry].
     *
     * @return int32 categoryId
     */
    int GetCategoryId(lua_State* L, SkillLineEntry* entry)
    {
        ALE::Push(L, entry->categoryId);
        return 1;
    }

    /**
     * Returns the display name of the [SkillLineEntry] (locale 0).
     *
     * @return string name
     */
    int GetName(lua_State* L, SkillLineEntry* entry)
    {
        ALE::Push(L, entry->name[0]);
        return 1;
    }

    /**
     * Returns the spell icon ID of the [SkillLineEntry].
     *
     * @return uint32 spellIcon
     */
    int GetSpellIcon(lua_State* L, SkillLineEntry* entry)
    {
        ALE::Push(L, entry->spellIcon);
        return 1;
    }

    /**
     * Returns whether this skill line can be linked (e.g. professions with recipes).
     *
     * @return uint32 canLink
     */
    int GetCanLink(lua_State* L, SkillLineEntry* entry)
    {
        ALE::Push(L, entry->canLink);
        return 1;
    }
}
#endif
