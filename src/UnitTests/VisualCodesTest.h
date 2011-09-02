#ifndef VISUALCODESTEST_H_INCLUDED
#define VISUALCODESTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <VisualCodes.h>

class VisualCodesTest: public CxxTest::TestSuite
{
public:
    void TestHuman()
    {
        TS_ASSERT(GetMesh(VC::LIVE | VC::ANIMAL | VC::HUMAN) == "robot.mesh");
    }

    void TestGrass()
    {
        TS_ASSERT(GetMesh(VC::LIVE | VC::PLANT) == "Grass.mesh");
    }

    void TestZebra()
    {
        TS_ASSERT(GetMesh(VC::LIVE | VC::ANIMAL | VC::HERBIVORES) == "zebra.mesh");
    }

};

#endif
