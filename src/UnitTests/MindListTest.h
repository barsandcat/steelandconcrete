#ifndef MINDLISTTEST_H_INCLUDED
#define MINDLISTTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <MindList.h>

class MindListTest: public CxxTest::TestSuite
{
public:
    void setUp()
    {
        MindList::Clear();
    }

    void tearDown()
    {
        MindList::Clear();
    }

    void TestBase()
    {

        MindList::NewMind(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), size_t(1));
        MindList::DeleteMind(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), size_t(0));
    }

    void TestUpdate()
    {
        MindList::NewMind(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), size_t(1));
        MindList::UpdateMinds(1);
        TS_ASSERT_EQUALS(MindList::GetSize(), size_t(0));
    }
};


#endif // MINDLISTTEST_H_INCLUDED

