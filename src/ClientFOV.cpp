#include <pch.h>

#include <ClientFOV.h>
#include <ServerTile.h>
#include <UnitList.h>

ClientFOV::ClientFOV(INetwork& aNetwork, const ServerGeodesicGrid::Tiles& aTiles, UnitId aAvatarId):
    mAvatarId(aAvatarId), mNetwork(aNetwork), mTiles(aTiles)
{
}

ClientFOV::~ClientFOV()
{
    //dtor
}

void ClientFOV::AddShowTile(PayloadMsg& aResponse, TileId aTileId)
{
    ChangeMsg* change = aResponse.add_changes();
    ShowTileMsg* showTile = change->mutable_showtile();
    showTile->set_tileid(aTileId);
    const ServerTile& tile = *mTiles.at(aTileId);
    showTile->set_height(tile.GetHeight());
    showTile->set_whater(tile.GetWater());

    if (const UnitId unitId = tile.GetUnitId())
    {
        const ServerUnit* unit = UnitList::GetUnit(unitId);
        ChangeMsg* change = aResponse.add_changes();
        UnitEnterMsg* unitEnter = change->mutable_unitenter();
        unitEnter->set_unitid(unitId);
        unitEnter->set_to(aTileId);
        unitEnter->set_visualcode(unit->GetClass().GetVisualCode());
    }
}

void AddHideTile(PayloadMsg& aResponse, TileId aTileId)
{
    ChangeMsg* change = aResponse.add_changes();
    HideTileMsg* hideTile = change->mutable_hidetile();
    hideTile->set_tileid(aTileId);
}

std::set<TileId> ClientFOV::GetVisibleTiles(int aDepth)
{
    std::set<TileId> result;
    std::set<TileId> toIterate;
    ServerTile& tile = UnitList::GetUnit(mAvatarId)->GetUnitTile();
    toIterate.insert(tile.GetTileId());
    result.insert(tile.GetTileId());

    for (int d = 0; d < aDepth; ++d)
    {
        std::set<TileId> newTiles;
        for (std::set<TileId>::iterator i = toIterate.begin(); i != toIterate.end(); ++i)
        {
            ServerTile* tile = mTiles.at(*i);
            for (size_t n = 0; n < tile->GetNeighbourCount(); ++n)
            {
                const TileId tileId = tile->GetNeighbour(n).GetTileId();
                if (result.find(tileId) == result.end())
                {
                    newTiles.insert(tileId);
                }
            }
        }
        toIterate = newTiles;
        result.insert(newTiles.begin(), newTiles.end());
    }

    return result;
}


void ClientFOV::SendUpdate(const GameTime aServerTime, const GameTime aClientTime,
                           const GameTime aTimeStep, const int32 aVisionRadius,
                           const int32 aGameUpdateLength)
{
    std::set<TileId> currentVisibleTiles = GetVisibleTiles(aVisionRadius);

    const int32 toSend = (aServerTime - aClientTime) / aTimeStep;
    const bool outOfBounds = aClientTime <= 0 || toSend >= ChangeList::mSize;
    if (!outOfBounds)
    {
        //GetLog() << "Show new tiles";
        std::vector<TileId> newVisibleTiles(currentVisibleTiles.size());
        std::vector<TileId>::iterator newVisibleEnd = std::set_difference(
            currentVisibleTiles.begin(), currentVisibleTiles.end(),
            mVisibleTiles.begin(), mVisibleTiles.end(), newVisibleTiles.begin());

        std::vector<TileId> newHiddenTiles(mVisibleTiles.size());
        std::vector<TileId>::iterator newHiddenEnd = std::set_difference(
            mVisibleTiles.begin(), mVisibleTiles.end(),
            currentVisibleTiles.begin(), currentVisibleTiles.end(), newHiddenTiles.begin());

        if (newVisibleTiles.begin() != newVisibleEnd || newHiddenTiles.begin() != newHiddenEnd)
        {
            PayloadMsg response;
            response.set_last(false);

            std::vector<TileId>::iterator n;
            for (n = newVisibleTiles.begin(); n != newVisibleEnd; ++n)
            {
                AddShowTile(response, *n);
            }

            for (n = newHiddenTiles.begin(); n != newHiddenEnd; ++n)
            {
                AddHideTile(response, *n);
            }

            mNetwork.WriteMessage(response);
        }

        // send events
        for (int32 t = toSend - 1; t >= 0; --t)
        {
            for (std::set<TileId>::iterator n = currentVisibleTiles.begin(); n != currentVisibleTiles.end(); ++n)
            {
                const TileId id = *n;
                ServerTile* tile = mTiles.at(id);
                tile->GetChangeList()->Write(mNetwork, t, currentVisibleTiles);
            }
        }
    }
    else
    {
        //GetLog() << "Send everything in view";
        PayloadMsg msg;
        msg.set_last(false);
        for (std::set<TileId>::iterator n = currentVisibleTiles.begin(); n != currentVisibleTiles.end(); ++n)
        {
            AddShowTile(msg, *n);
        }
        mNetwork.WriteMessage(msg);
    }

    // set time
    PayloadMsg emptyMsg;
    emptyMsg.set_last(true);
    emptyMsg.set_time(aServerTime);
    emptyMsg.set_update_length(aGameUpdateLength);
    mNetwork.WriteMessage(emptyMsg);

    mVisibleTiles = currentVisibleTiles;
}
