#include <pch.h>
#include <ClientGame.h>
#include <ClientApp.h>
#include <Unit.pb.h>
#include <Network.h>
#include <ClientLog.h>

ClientGame::ClientGame(ClientApp& aApp, socket_t& aSocket): mApp(aApp), mSocket(aSocket), mGrid(aSocket)
{
    UnitCountMsg unitCount;
    ReadMessage(aSocket, unitCount);
    GetLog() << "Recived unit count " << unitCount.ShortDebugString();

    for (size_t i = 0; i < unitCount.count(); ++i)
    {
        UnitMsg unit;
        ReadMessage(aSocket, unit);
        mUnits.insert(std::make_pair(
            unit.tag(),
            new ClientUnit(mGrid.GetTile(unit.tile()), unit.tag())
        ));
    }
    GetLog() << "Recived all units";
}

ClientGame::~ClientGame()
{
    mSocket.close();
    delete &mSocket;
}

void ClientGame::CreateUnitEntities(Ogre::SceneManager& aSceneManager) const
{
    std::map< int, ClientUnit* >::const_iterator i = mUnits.begin();
    for (; i != mUnits.end(); ++i)
    {
        i->second->CreateEntity(aSceneManager);
    }
}
