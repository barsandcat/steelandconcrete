#ifndef MINDLISTTEST_H_INCLUDED
#define MINDLISTTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <MindList.h>

class MindListTest: public CxxTest::TestSuite
{
public:
    void TestBase()
    {
        MindList::NewMind(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), 1);
        MindList::DeleteMind(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), 0);
    }

    void TestUpdate()
    {
        MindList::NewMind(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), 1);
        MindList::UpdateMinds(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), 0);
    }
};


#endif // MINDLISTTEST_H_INCLUDED

