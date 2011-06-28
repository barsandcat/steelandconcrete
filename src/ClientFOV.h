#ifndef CLIENTFOV_H
#define CLIENTFOV_H

#include<INetwork.h>
#include<ServerGame.h>
#include<Typedefs.h>
#include<IChange.h>
#include<boost/noncopyable.hpp>

class ClientFOV: public boost::noncopyable
{
public:
    ClientFOV(INetwork& aNetwork, ServerGame& aGame, UnitId aAvatarId);
    ~ClientFOV();
    void SendUpdate(GameTime aClientTime);
private:
    std::set<TileId> GetVisibleTiles(int aDepth);
    const UnitId mAvatarId;
    INetwork& mNetwork;
    ServerGame& mGame;
    std::set<TileId> mVisibleTiles;

};

#endif // CLIENTFOV_H
