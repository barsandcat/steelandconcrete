#ifndef NETWORKTEST_H_INCLUDED
#define NETWORKTEST_H_INCLUDED

// MyTestSuite.h
#include <cxxtest/TestSuite.h>
#include <ServerGeodesicGrid.h>
#include <proto/Tile.pb.h>
#include <proto/Vector.pb.h>

#include <unisock.h>

#include <iostream>
#include <cstdlib>
#include <DummySocket.h>
#include <ChangeList.h>
#include <Network.h>

class MyTestSuite : public CxxTest::TestSuite
{
public:
    void setUp()
    {
        mNetwork = new Network(new DummySocket());
    }

    void tearDown()
    {
        delete mNetwork;
    }

    void TestChangeList()
    {
        for (int j = 0; j < 10; ++j)
        {
            ChangeList::Clear();
            for (int i = 0; i < 50 * j; ++i)
            {
                ChangeList::AddRemove(1);
            }
            TS_ASSERT_THROWS_NOTHING(ChangeList::Write(*mNetwork, 0));
        }
    }

    void TestGeodesicGridSave()
    {
        ServerGeodesicGrid grid1(0, 5000);
        grid1.Save("test.gg");
        ServerGeodesicGrid grid2("test.gg");

        TS_ASSERT_EQUALS(grid1.GetTileCount(), grid2.GetTileCount());
        TS_ASSERT_EQUALS(grid1.GetEdgeCount(), grid2.GetEdgeCount());
        for (size_t i = 0; i < grid1.GetTileCount(); ++i)
        {
            ServerTile& tile1 = grid1.GetTile(i);
            ServerTile& tile2 = grid2.GetTile(i);
            TS_ASSERT(tile1.GetPosition().positionCloses(tile2.GetPosition(), 0.00001f));
            TS_ASSERT_EQUALS(tile1.GetTileId(), i);
            TS_ASSERT_EQUALS(tile2.GetTileId(), i);
            TS_ASSERT_EQUALS(tile1.GetNeighbourCount(), tile2.GetNeighbourCount());
            for (size_t j = 0; j < tile1.GetNeighbourCount(); ++j)
            {
                TS_ASSERT_EQUALS(tile1.GetNeighbour(j).GetTileId(), tile2.GetNeighbour(j).GetTileId());
            }
        }
    }
private:
    Network* mNetwork;

};

#endif
