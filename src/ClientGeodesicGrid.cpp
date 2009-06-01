#include <pch.h>
#include <ClientGeodesicGrid.h>
#include <GeodesicGrid.pb.h>
#include <Network.h>
#include <ClientLog.h>
#include <ClientApp.h>

ClientGeodesicGrid::~ClientGeodesicGrid()
{
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        delete mEdges[i];
        mEdges[i] = NULL;
    }
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        delete mTiles[i];
        mTiles[i] = NULL;
    }
}

Ogre::ManualObject* ClientGeodesicGrid::ConstructDebugMesh() const
{
    Ogre::ManualObject* manual = new Ogre::ManualObject("PlanetDebugManual");
    manual->begin("Grassland", Ogre::RenderOperation::OT_LINE_LIST);
    for (size_t i = 0; i < mEdges.size(); ++i)
    {
        ClientEdge* edge = mEdges[i];
        manual->position(edge->GetTileA()->GetPosition());
        manual->position(edge->GetTileB()->GetPosition());
    }
    manual->end();
    return manual;
}

Ogre::StaticGeometry* ClientGeodesicGrid::ConstructStaticGeometry() const
{
    Ogre::SceneManager& aSceneManager = ClientApp::GetSceneMgr();
    Ogre::StaticGeometry* staticPlanet = aSceneManager.createStaticGeometry("Planet.Static");
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        Ogre::String indexName = Ogre::StringConverter::toString(i);
        Ogre::String meshName = indexName + "ClientTile.mesh";
        // Create entity
        Ogre::ResourcePtr tileMesh = mTiles[i]->ConstructMesh(meshName);
        Ogre::Entity* tileEntity = aSceneManager.createEntity(indexName + "ClientTile.entity", meshName);
        // Pack it in
        staticPlanet->addEntity(tileEntity, Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY);
        // Clean up
        aSceneManager.destroyEntity(tileEntity);
    }
    staticPlanet->build();
    return staticPlanet;
}

void ClientGeodesicGrid::InitTiles()
{
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        mTiles[i]->SortNeighbourhood();
        mTiles[i]->SetIndex(i);
    }
}

ClientGeodesicGrid::ClientGeodesicGrid(socket_t& aSocket, LoadingSheet& loadingSheet)
{
    GeodesicGridSizeMsg gridInfo;
    ReadMessage(aSocket, gridInfo);
    GetLog() << "Recived grid info " << gridInfo.ShortDebugString();
    mTiles.resize(gridInfo.tilecount());
    mEdges.resize(gridInfo.edgecount());

    for (size_t i = 0; i < gridInfo.tilecount(); ++i)
    {
        TileMsg tile;
        ReadMessage(aSocket, tile);
        mTiles[tile.tag()] = new ClientTile(Ogre::Vector3(tile.position().x(), tile.position().y(), tile.position().z()));
    }
    GetLog() << "Recived all tiles";
    loadingSheet.SetProgress(10);

    for (size_t i = 0; i < gridInfo.edgecount(); ++i)
    {
        EdgeMsg edge;
        ReadMessage(aSocket, edge);
        mEdges[i] = new ClientEdge(mTiles[edge.tilea()], mTiles[edge.tileb()]);
    }
    GetLog() << "Recived all edges ";
    loadingSheet.SetProgress(50);

    InitTiles();
}
