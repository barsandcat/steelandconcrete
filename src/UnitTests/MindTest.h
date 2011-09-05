#ifndef MINDTEST_H_INCLUDED
#define MINDTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>

class MindTest: public CxxTest::TestSuite
{
public:
    void TestBase()
    {
        TS_ASSERT(true);
    }
};


#endif // MINDTEST_H_INCLUDED

