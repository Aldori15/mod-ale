/*
* Copyright (C) 2010 - 2024 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef LOOTMETHODS_H
#define LOOTMETHODS_H

/***
 * Represents loot data for items, gold, and other rewards dropped by creatures or objects.
 *
 * Inherits all methods from: none
 */
namespace LuaLoot
{
    /**
     * Returns 'true' if the [Loot] has been looted, 'false' otherwise
     *
     * @return bool isLooted
     */
    int IsLooted(lua_State* L, Loot* loot)
    {
        Eluna::Push(L, loot->isLooted());
        return 1;
    }

    /**
     * Adds an [Item] to the [Loot] with specified parameters
     *
     * @param uint32 itemid
     * @param float chance
     * @param bool needs_quest = false
     * @param uint16 loot_mode
     * @param uint8 groupid
     * @param uint8 min_count
     * @param uint8 max_count
     */
    int AddItem(lua_State* L, Loot* loot)
    {
        uint32 itemid = Eluna::CHECKVAL<uint32>(L, 2);

        float chance = Eluna::CHECKVAL<float>(L, 3);
        bool needs_quest = Eluna::CHECKVAL<bool>(L, 4, false);

        uint16 loot_mode = Eluna::CHECKVAL<uint16>(L, 5);
        uint8 groupid = Eluna::CHECKVAL<uint8>(L, 6);

        uint8 min_count = Eluna::CHECKVAL<uint8>(L, 7);
        uint8 max_count = Eluna::CHECKVAL<uint8>(L, 8);


        for (LootItem &lootitem : loot->items)
        {
            if (lootitem.itemid == itemid && lootitem.count < 255)
            {
                lootitem.count += min_count; // Todo: Update to get rand between min_count and max_count
                return 0;
            }
        }

        LootStoreItem newLootStoreItem(itemid, 0, chance, needs_quest, loot_mode, groupid, min_count, max_count);
        loot->AddItem(newLootStoreItem);

        return 0;
    }

    /**
     * Returns 'true' if the [Loot] contains the specified [Item], 'false' otherwise
     *
     * @param uint32 itemid
     * @param uint32 count
     * @return bool hasItem
     */
    int HasItem(lua_State* L, Loot* loot)
    {
        uint32 itemid = Eluna::CHECKVAL<uint32>(L, 2, false);
        uint32 count = Eluna::CHECKVAL<uint32>(L, 3, false);
        bool has_item = false;

        if (itemid)
        {
            for (const LootItem &lootitem : loot->items)
            {
                if (lootitem.itemid == itemid && (count == 0 || lootitem.count == count))
                {
                    has_item = true;
                    break;
                }
            }
        }
        else
        {
            for (const LootItem &lootitem : loot->items)
            {
                if (lootitem.itemid && lootitem.itemid != 0)
                {
                    has_item = true;
                    break;
                }
            }
        }
        Eluna::Push(L, has_item);
        return 1;
    }

    /**
     * Removes an [Item] from the [Loot], optionally reducing count
     *
     * @param uint32 itemid
     * @param bool isCountSpecified
     * @param uint32 count : required only if isCountSpecified is true
     */
    int RemoveItem(lua_State* L, Loot* loot)
    {
        uint32 itemid = Eluna::CHECKVAL<uint32>(L, 2);
        bool isCountSpecified = Eluna::CHECKVAL<uint32>(L, 3, false);
        uint32 count = isCountSpecified ? Eluna::CHECKVAL<uint32>(L, 4) : 0;

        for (auto it = loot->items.begin(); it != loot->items.end();)
        {
            if (it->itemid == itemid)
            {
                if (isCountSpecified)
                {
                    if (it->count > count)
                    {
                        it->count -= count;
                        break;
                    }
                    else
                    {
                        count -= it->count;
                    }
                }

                it = loot->items.erase(it);
            }
            else
            {
                ++it;
            }
        }

        return 0;
    }

    /**
     * Returns the amount of gold in the [Loot]
     *
     * @return uint32 gold
     */
    int GetMoney(lua_State* L, Loot* loot)
    {
        Eluna::Push(L, loot->gold);
        return 1;
    }

    /**
     * Sets the amount of gold in the [Loot]
     *
     * @param uint32 money
     */
    int SetMoney(lua_State* L, Loot* loot)
    {
        uint32 money = Eluna::CHECKVAL<uint32>(L, 2);

        loot->gold = money;
        return 0;
    }

    /**
     * Generates a random gold value for the [Loot] between min and max
     *
     * @param uint32 min
     * @param uint32 max
     */
    int GenerateMoney(lua_State* L, Loot* loot)
    {
        uint32 min = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 max = Eluna::CHECKVAL<uint32>(L, 3);

        loot->generateMoneyLoot(min, max);
        return 0;
    }

    /**
     * Clears all items and gold from the [Loot]
     */
    int Clear(lua_State* /*L*/, Loot* loot)
    {
        loot->clear();
        return 0;
    }

    /**
     * Sets the unlooted count for the [Loot]
     *
     * @param uint32 count
     */
    int SetUnlootedCount(lua_State* L, Loot* loot)
    {
        uint32 count = Eluna::CHECKVAL<uint32>(L, 2);

        loot->unlootedCount = count;
        return 0;
    }

    /**
     * Returns the number of unlooted items in the [Loot]
     *
     * @return uint32 unlootedCount
     */
    int GetUnlootedCount(lua_State* L, Loot* loot)
    {
        Eluna::Push(L, loot->unlootedCount);
        return 0;
    }

    /**
     * Returns a table of items in the [Loot] with keys: 
     * id, index, count, needs_quest, is_looted, roll_winner_guid
     *
     * @return table items
     */
    int GetItems(lua_State* L, Loot* loot)
    {
        lua_createtable(L, loot->items.size(), 0);
        int tbl = lua_gettop(L);

        for (unsigned int i = 0; i < loot->items.size(); i++)
        {
            lua_newtable(L);

            Eluna::Push(L, loot->items[i].itemid);
            lua_setfield(L, -2, "id");

            Eluna::Push(L, loot->items[i].itemIndex);
            lua_setfield(L, -2, "index");

            Eluna::Push(L, loot->items[i].count);
            lua_setfield(L, -2, "count");

            Eluna::Push(L, loot->items[i].needs_quest);
            lua_setfield(L, -2, "needs_quest");

            Eluna::Push(L, loot->items[i].is_looted);
            lua_setfield(L, -2, "is_looted");

            Eluna::Push(L, loot->items[i].rollWinnerGUID);
            lua_setfield(L, -2, "roll_winner_guid");

            lua_rawseti(L, tbl, i + 1);
        }

        lua_settop(L, tbl);
        return 1;
    }

    /**
     * Updates each item's index in the [Loot] to match its current order
     */
    int UpdateItemIndex(lua_State* /*L*/, Loot* loot)
    {
        for (unsigned int i = 0; i < loot->items.size(); i++)
            loot->items[i].itemIndex = i;

        return 0;
    }

    /**
     * Sets the 'looted' flag on an [Item] in the [Loot]
     *
     * @param uint32 itemid
     * @param uint32 count
     * @param bool looted = true
     */
    int SetItemLooted(lua_State* L, Loot* loot)
    {
        uint32 itemid = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 count = Eluna::CHECKVAL<uint32>(L, 3);
        bool looted = Eluna::CHECKVAL<uint32>(L, 4, true);

        for (auto &lootItem : loot->items)
        {
            if (lootItem.itemid == itemid && (count == 0 || lootItem.count == count))
            {
                lootItem.is_looted = looted;
                break;
            }
        }

        return 0;
    }
}
#endif