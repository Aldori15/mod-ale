#ifndef AUCTIONENTRYMETHODS_H
#define AUCTIONENTRYMETHODS_H

#include "LuaEngine.h"
#include "AuctionHouseMgr.h"


/***
 * Represents a live auction listing, including the listed item, seller, bid state, buyout value, deposit, and expiration data.
 */
namespace LuaAuctionEntry
{
    /**
     * Returns the auction ID
     * @return uint32 auctionId
     */
    int GetId(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->Id);
        return 1;
    }

    /**
     * Returns the auction house ID
     * @return uint8 houseId : AuctionHouseId enum (2=Alliance, 6=Horde, 7=Neutral)
     */
    int GetHouseId(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, static_cast<uint8>(auction->GetHouseId()));
        return 1;
    }

    /**
     * Returns the item GUID
     * @return ObjectGuid itemGuid
     */
    int GetItemGuid(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->item_guid);
        return 1;
    }

    /**
     * Returns the item template/entry ID
     * @return uint32 itemEntry
     */
    int GetItemEntry(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->item_template);
        return 1;
    }

    /**
     * Returns the item count
     * @return uint32 itemCount
     */
    int GetItemCount(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->itemCount);
        return 1;
    }

    /**
     * Returns the owner GUID (seller)
     * @return ObjectGuid ownerGuid
     */
    int GetOwnerGuid(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->owner);
        return 1;
    }

    /**
     * Returns the owner as Player object (if online)
     * @return Player owner or nil
     */
    int GetOwner(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, ObjectAccessor::FindPlayer(auction->owner));
        return 1;
    }

    /**
     * Returns the starting bid
     * @return uint32 startBid
     */
    int GetStartBid(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->startbid);
        return 1;
    }

    /**
     * Returns the current bid
     * @return uint32 currentBid
     */
    int GetBid(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->bid);
        return 1;
    }

    /**
     * Returns the buyout price
     * @return uint32 buyout
     */
    int GetBuyout(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->buyout);
        return 1;
    }

    /**
     * Returns the expire time (Unix timestamp)
     * @return uint32 expireTime
     */
    int GetExpireTime(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, static_cast<uint32>(auction->expire_time));
        return 1;
    }

    /**
     * Returns seconds until auction expires
     * @return int32 timeLeft (negative if expired)
     */
    int GetTimeLeft(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, static_cast<int32>(auction->expire_time - time(nullptr)));
        return 1;
    }

    /**
     * Returns the current bidder GUID
     * @return ObjectGuid bidderGuid
     */
    int GetBidderGuid(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->bidder);
        return 1;
    }

    /**
     * Returns the current bidder as Player object (if online)
     * @return Player bidder or nil
     */
    int GetBidder(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, ObjectAccessor::FindPlayer(auction->bidder));
        return 1;
    }

    /**
     * Returns the deposit amount
     * @return uint32 deposit
     */
    int GetDeposit(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->deposit);
        return 1;
    }

    /**
     * Returns the auction house cut (fee)
     * @return uint32 cut
     */
    int GetAuctionCut(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->GetAuctionCut());
        return 1;
    }

    /**
     * Returns the minimum outbid amount
     * @return uint32 outBid
     */
    int GetAuctionOutBid(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->GetAuctionOutBid());
        return 1;
    }

    /**
     * Returns the Item object from auction house cache
     * @return Item item or nil
     */
    int GetItem(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, sAuctionMgr->GetAItem(auction->item_guid));
        return 1;
    }

    /**
     * Checks if auction has a bidder
     * @return bool hasBidder
     */
    int HasBidder(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, !auction->bidder.IsEmpty());
        return 1;
    }

    /**
     * Checks if auction is expired
     * @return bool isExpired
     */
    int IsExpired(lua_State* L, AuctionEntry* auction)
    {
        ALE::Push(L, auction->expire_time <= time(nullptr));
        return 1;
    }

    /**
     * Builds mail subject for auction mail
     * @param uint32 response : MailAuctionAnswers enum
     * @return string subject
     */
    int BuildMailSubject(lua_State* L, AuctionEntry* auction)
    {
        uint32 response = ALE::CHECKVAL<uint32>(L, 2);
        ALE::Push(L, auction->BuildAuctionMailSubject(static_cast<MailAuctionAnswers>(response)));
        return 1;
    }
};
#endif // AUCTIONENTRYMETHODS_H
