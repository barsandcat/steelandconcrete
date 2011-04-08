#ifndef GEODESICGRIDTEST_H_INCLUDED
#define GEODESICGRIDTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>

#include <ServerGeodesicGrid.h>
#include <CompareEdgesAngles.h>

class GeodesicGridTest: public CxxTest::TestSuite
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }
    void TestGeodesicGrid()
    {
        ServerGeodesicGrid grid1(0, 5000);
        const Ogre::Real len = grid1.GetTile(0).GetPosition().length();

        for (size_t i = 0; i < grid1.GetTileCount(); ++i)
        {
            ServerTile& tile1 = grid1.GetTile(i);
            TS_ASSERT_DELTA(len, tile1.GetPosition().length(), 0.00001f);
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
    void TestCompareAngles()
    {
        Ogre::Vector3 root(0,           0.52573108,  0.850650787);
        Ogre::Vector3 pole(0.309016973, 0.809016943, 0.49999997);
        CompareEdgesAngles<ServerTile> compare(root, pole);
        Ogre::Vector3 u(-0.309016973, 0.809016943, 0.49999997);
        Ogre::Radian angle = compare.CalcAngle(u);
        TS_ASSERT(angle == Ogre::Radian(5.02654839));
    }
    void TestCompareAngles2()
    {
        Ogre::Vector3 root(0,           0.52573108,  0.850650787);
        Ogre::Vector3 pole(0.309016973, 0.809016943, 0.49999997);
        root *= 10.0;
        pole *= 10.0;
        CompareEdgesAngles<ServerTile> compare(root, pole);
        Ogre::Vector3 u(-0.309016973, 0.809016943, 0.49999997);
        u *= 10.0;
        Ogre::Radian angle = compare.CalcAngle(u);
        TS_ASSERT(angle == Ogre::Radian(5.02654839));
    }

};


#endif // GEODESICGRIDTEST_H_INCLUDED
