#include <pch.h>

#include <ClientFOV.h>
#include <ServerTile.h>
#include <UnitList.h>

ClientFOV::ClientFOV(INetwork& aNetwork, ServerGame& aGame, UnitId aAvatarId):
    mAvatarId(aAvatarId), mNetwork(aNetwork), mGame(aGame)
{
    mVisibleTiles = GetVisibleTiles();
    SendUpdate(0);
}

ClientFOV::~ClientFOV()
{
    //dtor
}

void ClientFOV::AddShowTile(ResponseMsg& aResponse, TileId aTileId)
{
    ChangeMsg* change = aResponse.add_changes();
    ShowTileMsg* showTile = change->mutable_showtile();
    showTile->set_tileid(aTileId);
}

std::set<TileId> ClientFOV::GetVisibleTiles()
{
    std::set<TileId> result;
    ServerTile& tile = UnitList::GetUnit(mAvatarId)->GetPosition();
    result.insert(tile.GetTileId());
    for (size_t n = 0; n < tile.GetNeighbourCount(); ++n)
    {
        result.insert(tile.GetNeighbour(n).GetTileId());
    }
    return result;
}


void ClientFOV::SendUpdate(GameTime aClientTime)
{
    // show tiles
    std::set<TileId> currentVisibleTiles = GetVisibleTiles();
    std::vector<TileId> newVisibleTiles(currentVisibleTiles.size());
    std::vector<TileId>::iterator end = std::set_difference(
                                            currentVisibleTiles.begin(), currentVisibleTiles.end(),
                                            mVisibleTiles.begin(), mVisibleTiles.end(), newVisibleTiles.begin());

    ResponseMsg response;
    response.set_type(RESPONSE_PART);
    for (std::vector<TileId>::iterator n = newVisibleTiles.begin(); n != end; ++n)
    {
        AddShowTile(response, *n);
    }
    mNetwork.WriteMessage(response);
    mVisibleTiles = currentVisibleTiles;

    // send events
    const int32 toSend = (mGame.GetTime() - aClientTime) / mGame.GetTimeStep();
    for (int32 t = toSend - 1; t >= 0; --t)
    {
        for (std::set<TileId>::iterator n = mVisibleTiles.begin(); n != mVisibleTiles.end(); ++n)
        {
            const TileId id = *n;
            const ServerTile* tile = mGame.GetTiles().at(id);
            tile->GetChangeList()->Write(mNetwork, t, mVisibleTiles);
        }
    }

    // set time
    ResponseMsg emptyMsg;
    emptyMsg.set_type(RESPONSE_OK);
    emptyMsg.set_time(ServerGame::GetTime());
    emptyMsg.set_update_length(mGame.GetUpdateLength());
    mNetwork.WriteMessage(emptyMsg);
}
