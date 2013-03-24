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
        ServerGeodesicGrid::Tiles tiles;
        ServerGeodesicGrid grid1(tiles, 0);
        const Ogre::Real len = tiles.at(0)->GetPosition().length();

        for (size_t i = 0; i < tiles.size(); ++i)
        {
            ServerTile& tile1 = *tiles.at(i);
            TS_ASSERT_DELTA(len, tile1.GetPosition().length(), 0.00001f);
            const size_t tileCount = tile1.GetNeighbourCount();
            TS_ASSERT(tileCount == 5 || tileCount == 6);
            const Ogre::Real dist = (tile1.GetPosition() - tile1.GetNeighbour(0).GetPosition()).length();
            for (size_t k = 0; k < tileCount; ++k)
            {
                const Ogre::Real curdist = (tile1.GetPosition() - tile1.GetNeighbour(k).GetPosition()).length();
                TS_ASSERT_DELTA(dist, curdist, dist * 0.15f);
            }
        }
    }

    void TestCompareAngles()
    {
        Ogre::Vector3 root(0.0f,           0.52573108f,  0.850650787f);
        Ogre::Vector3 pole(0.309016973f, 0.809016943f, 0.49999997f);
        CompareEdgesAngles<ServerTile> compare(root, pole);
        Ogre::Vector3 u(-0.309016973f, 0.809016943f, 0.49999997f);
        Ogre::Radian angle = compare.CalcAngle(u);
        TS_ASSERT(angle == Ogre::Radian(5.02654839f));
    }


    void TestCompareAngles2()
    {
        std::vector<ServerTile*> m;
        m.push_back(new ServerTile(0, Ogre::Vector3( 09.11f,  23.83f,  14.73f)));
        m.push_back(new ServerTile(1, Ogre::Vector3(-09.11f,  23.83f,  14.73f)));
        m.push_back(new ServerTile(2, Ogre::Vector3( 14.73f,  09.11f,  23.84f)));
        m.push_back(new ServerTile(3, Ogre::Vector3(-14.73f,  09.11f,  23.84f)));
        m.push_back(new ServerTile(4, Ogre::Vector3( 00.00f,  15.49f,  25.07f)));

        Ogre::Vector3 root(00.00f,  15.49f,  25.07f);

        CompareEdgesAngles<ServerTile> compare(root, m.at(0)->GetPosition());
        std::sort(m.begin() + 1, m.end(), compare);

        TS_ASSERT_EQUALS(m[0]->GetTileId(), TileId(0));
        TS_ASSERT_EQUALS(m[1]->GetTileId(), TileId(2));
        TS_ASSERT_EQUALS(m[2]->GetTileId(), TileId(4));
        TS_ASSERT_EQUALS(m[3]->GetTileId(), TileId(3));
        TS_ASSERT_EQUALS(m[4]->GetTileId(), TileId(1));
    }

};


#endif // GEODESICGRIDTEST_H_INCLUDED
