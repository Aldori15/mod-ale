#ifndef AUCTIONHOUSEMETHODS_H
#define AUCTIONHOUSEMETHODS_H

#include "LuaEngine.h"
#include "AuctionHouseMgr.h"
#include "AuctionHouseSearcher.h"
#include "Item.h"
#include "Player.h"
#include "World.h"
#include "CharacterCache.h"


/***
 * Represents an auction house entry and provides methods for deposit calculation, auction creation, bid placement, and auction management for that house.
 */
namespace LuaAuctionHouse
{
    /**
     * @brief Calculates the required deposit for listing an item.
     * @param uint32 time - Auction duration in hours (e.g., 12, 24, 48).
     * @param Item item - The item object pointer to be auctioned.
     * @param uint32 count - The number of items in the stack.
     * @return uint32 - The calculated deposit amount in copper coins.
     */
    int GetDeposit(lua_State* L, AuctionHouseEntry* entry)
    {
        uint32 time  = ALE::CHECKVAL<uint32>(L, 2);
        Item* item   = ALE::CHECKVAL<Item*>(L, 3);
        uint32 count = ALE::CHECKVAL<uint32>(L, 4);

        if (!item)
            return 0;

        ALE::Push(L, sAuctionMgr->GetAuctionDeposit(entry, time, item, count));
        return 1;
    }

    /**
     * @brief Registers an item in the AuctionHouseMgr's internal tracking map.
     * @param Item item - The item object pointer to add.
     */
    int AddAItem(lua_State* L, AuctionHouseEntry* /*entry*/)
    {
        Item* item = ALE::CHECKVAL<Item*>(L, 2);
        if (item)
            sAuctionMgr->AddAItem(item);
        return 0;
    }

    /**
     * @brief Removes an item from the AuctionHouseMgr's internal tracking map.
     * @param ObjectGuid guid - The unique GUID of the item to remove.
     * @param bool deleteFromDB - If true, the item instance will also be deleted from the database.
     * @return bool - True if the item was found and removed from the map.
     */
    int RemoveAItem(lua_State* L, AuctionHouseEntry* /*entry*/)
    {
        ObjectGuid guid = ALE::CHECKVAL<ObjectGuid>(L, 2);
        bool deleteFromDB = ALE::CHECKVAL<bool>(L, 3);

        if (deleteFromDB)
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            bool res = sAuctionMgr->RemoveAItem(guid, true, &trans);
            CharacterDatabase.CommitTransaction(trans);
            ALE::Push(L, res);
        }
        else
        {
            ALE::Push(L, sAuctionMgr->RemoveAItem(guid, false));
        }
        return 1;
    }

    /**
     * @brief Retrieves the Auction House DBC entry based on a faction template.
     * @param uint32 factionTemplateId - The faction template ID of the player or creature.
     * @return AuctionHouseEntry* - Pointer to the DBC entry or nil if not found.
     */
    int GetEntryFromFaction(lua_State* L, AuctionHouseEntry* /*entry*/)
    {
        uint32 factionTemplateId = ALE::CHECKVAL<uint32>(L, 2);
        AuctionHouseEntry const* entry = sAuctionMgr->GetAuctionHouseEntryFromFactionTemplate(factionTemplateId);
        ALE::Push(L, entry);
        return 1;
    }

    /**
     * @brief Retrieves the Auction House DBC entry based on its House ID.
     * @param uint32 houseId - The House ID (enum AuctionHouseId).
     * @return AuctionHouseEntry* - Pointer to the DBC entry or nil if not found.
     */
    int GetEntryFromHouse(lua_State* L, AuctionHouseEntry* /*entry*/)
    {
        uint32 houseId = ALE::CHECKVAL<uint32>(L, 2);
        AuctionHouseEntry const* entry = sAuctionMgr->GetAuctionHouseEntryFromHouse(AuctionHouseId(houseId));
        ALE::Push(L, entry);
        return 1;
    }

    /**
     * @brief Adds an auction entry to a specific auction house instance.
     * @param AuctionEntry auction - The auction entry object to add.
     */
    int AddAuction(lua_State* L, AuctionHouseEntry* entry)
    {
        AuctionEntry* auction = ALE::CHECKVAL<AuctionEntry*>(L, 2);

        if (!auction)
            return 0;

        AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(AuctionHouseId(entry->houseId));
        if (ah)
            ah->AddAuction(auction);

        return 0;
    }

    /**
     * @brief Removes an auction entry from a specific auction house instance.
     * @param AuctionEntry auction - The auction entry object to remove.
     * @return bool - True if the auction was found and successfully removed.
     */
    int RemoveAuction(lua_State* L, AuctionHouseEntry* entry)
    {
        AuctionEntry* auction = ALE::CHECKVAL<AuctionEntry*>(L, 2);

        if (!auction)
        {
            ALE::Push(L, false);
            return 1;
        }

        AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(AuctionHouseId(entry->houseId));
        if (ah)
            ALE::Push(L, ah->RemoveAuction(auction));
        else
            ALE::Push(L, false);

        return 1;
    }

    /**
    * Creates and registers a new auction entry.
    * @param Item item - The item object to be sold.
    * @param Player seller - The player who is selling the item.
    * @param uint32 bid - Starting bid in copper.
    * @param uint32 buyout - Buyout price in copper.
    * @param uint32 duration - Duration in hours.
    * @param uint32 deposit - Deposit paid in copper.
    * @return uint32 - The generated Auction ID, or 0 on failure.
    */
    int CreateNewAuction(lua_State* L, AuctionHouseEntry* entry)
    {
        // 1. Extract parameters from the Lua stack
        Item* item              = ALE::CHECKVAL<Item*>(L, 2);
        Player* seller          = ALE::CHECKVAL<Player*>(L, 3);
        uint32 bid              = ALE::CHECKVAL<uint32>(L, 4);
        uint32 buyout           = ALE::CHECKVAL<uint32>(L, 5);
        uint32 duration         = ALE::CHECKVAL<uint32>(L, 6);
        uint32 deposit          = ALE::CHECKVAL<uint32>(L, 7);

        // Basic object checks
        if (!item || !seller)
        {
            ALE::Push(L, 0);
            return 1;
        }

        // 2. Initialize system variables
        AuctionHouseId houseId = AuctionHouseId(entry->houseId);
        AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(houseId);
        AuctionHouseEntry const* ahEntry = sAuctionHouseStore.LookupEntry(entry->houseId);

        if (!ah || !ahEntry)
        {
            ALE::Push(L, 0);
            return 1;
        }

        seller->MoveItemFromInventory(item->GetBagSlot(), item->GetSlot(), true);

        // 3. Create auction record
        uint32 auctionTime = uint32(duration * HOUR * sWorld->getRate(RATE_AUCTION_TIME));
        AuctionEntry* AH = new AuctionEntry(); // AH is allocated here

        AH->Id = sObjectMgr->GenerateAuctionID();
        AH->houseId = houseId;
        AH->item_guid = item->GetGUID();
        AH->item_template = item->GetEntry();
        AH->itemCount = item->GetCount();
        AH->owner = seller->GetGUID();
        AH->startbid = bid;
        AH->bidder = ObjectGuid::Empty;
        AH->bid = 0;
        AH->buyout = buyout;
        AH->expire_time = GameTime::GetGameTime().count() + auctionTime;
        AH->deposit = deposit;
        AH->auctionHouseEntry = ahEntry;

        sAuctionMgr->AddAItem(item);
        ah->AddAuction(AH);

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        
        item->DeleteFromInventoryDB(trans); // Remove DB link to the bag/inventory
        item->SaveToDB(trans);              // Save item state to DB
        AH->SaveToDB(trans);                // Save the auction entry to DB

        seller->SaveInventoryAndGoldToDB(trans); // Save player's inventory and gold changes to DB
        
        CharacterDatabase.CommitTransaction(trans);

        ALE::Push(L, AH->Id);
        return 1;
    }

    /**
    * Places a remote bid or performs a buyout for an auction entry, bypassing distance/NPC checks.
    * @param Player player - The player object placing the bid.
    * @param uint32 auctionId - The unique ID of the auction entry to bid on.
    * @param uint32 price - The bid amount or buyout price in copper coins.
    * @return bool - True if the bid was successfully processed and saved to the database.
    */
    int PlaceBid(lua_State* L, AuctionHouseEntry* entry)
    {
        Player* player   = ALE::CHECKVAL<Player*>(L, 2);
        uint32 auctionId = ALE::CHECKVAL<uint32>(L, 3);
        uint32 price     = ALE::CHECKVAL<uint32>(L, 4);

        if (!player || !player->GetSession() || !auctionId || !price)
        {
            ALE::Push(L, false);
            return 1;
        }

        AuctionEntry* auction = nullptr;
        AuctionHouseObject* auctionHouse = nullptr;

        auto* ah = sAuctionMgr->GetAuctionsMapByHouseId(AuctionHouseId(entry->houseId));
        if (ah)
        {
            auction = ah->GetAuction(auctionId);
            if (auction)
            {
                auctionHouse = ah;
            }
        }

        if (!auction || !auctionHouse)
        {
            ALE::Push(L, false);
            return 1;
        }

        if (!sScriptMgr->OnPlayerCanPlaceAuctionBid(player, auction))
        {
            player->GetSession()->SendAuctionCommandResult(0, AUCTION_PLACE_BID, ERR_AUCTION_RESTRICTED_ACCOUNT);
            ALE::Push(L, false);
            return 1;
        }

        if (auction->owner == player->GetGUID())
        {
            player->GetSession()->SendAuctionCommandResult(0, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
            ALE::Push(L, false);
            return 1;
        }

        if (sCharacterCache->GetCharacterAccountIdByGuid(auction->owner) == player->GetSession()->GetAccountId())
        {
            player->GetSession()->SendAuctionCommandResult(0, AUCTION_PLACE_BID, ERR_AUCTION_BID_OWN);
            ALE::Push(L, false);
            return 1;
        }

        if (price <= auction->bid || price < auction->startbid)
        {
            ALE::Push(L, false);
            return 1;
        }

        if ((price < auction->buyout || auction->buyout == 0) &&
            price < auction->bid + AuctionEntry::CalculateAuctionOutBid(auction->bid))
        {
            ALE::Push(L, false);
            return 1;
        }

        if (!player->HasEnoughMoney((uint32)price))
        {
            ALE::Push(L, false);
            return 1;
        }

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        if (price < auction->buyout || auction->buyout == 0)
        {
            if (auction->bidder)
            {
                if (auction->bidder == player->GetGUID())
                    player->ModifyMoney(-int32(price - auction->bid));
                else
                {
                    sAuctionMgr->SendAuctionOutbiddedMail(auction, price, player, trans);
                    player->ModifyMoney(-int32(price));
                }
            }
            else
                player->ModifyMoney(-int32(price));

            auction->bidder = player->GetGUID();
            auction->bid = price;

            sAuctionMgr->GetAuctionHouseSearcher()->UpdateBid(auction);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID, price);

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_AUCTION_BID);
            stmt->SetData(0, auction->bidder.GetCounter());
            stmt->SetData(1, auction->bid);
            stmt->SetData(2, auction->Id);
            trans->Append(stmt);

            player->GetSession()->SendAuctionCommandResult(auction->Id, AUCTION_PLACE_BID, ERR_AUCTION_OK, 0);
        }
        else
        {
            if (player->GetGUID() == auction->bidder)
                player->ModifyMoney(-int32(auction->buyout - auction->bid));
            else
            {
                player->ModifyMoney(-int32(auction->buyout));
                if (auction->bidder)
                    sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, player, trans);
            }
            
            auction->bidder = player->GetGUID();
            auction->bid = auction->buyout;
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID, auction->buyout);

            sAuctionMgr->SendAuctionSalePendingMail(auction, trans);
            sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
            sAuctionMgr->SendAuctionWonMail(auction, trans);
            sScriptMgr->OnAuctionSuccessful(auctionHouse, auction);

            player->GetSession()->SendAuctionCommandResult(auction->Id, AUCTION_PLACE_BID, ERR_AUCTION_OK);

            auction->DeleteFromDB(trans);
            sAuctionMgr->RemoveAItem(auction->item_guid);
            auctionHouse->RemoveAuction(auction);
        }

        player->SaveInventoryAndGoldToDB(trans);
        CharacterDatabase.CommitTransaction(trans);

        ALE::Push(L, true);
        return 1;
    }
};
#endif // AUCTIONHOUSEMETHODS_H
