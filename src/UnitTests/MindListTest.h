#ifndef MINDLISTTEST_H_INCLUDED
#define MINDLISTTEST_H_INCLUDED

#include <cxxtest/TestSuite.h>
#include <MindList.h>
#include <Mind.h>
#include <Exceptions.h>

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

    void TestAvatar()
    {
        TS_ASSERT(!MindList::GetFreeMind());
        MindList::NewMind(1);
        Mind* mind = MindList::GetFreeMind();
        TS_ASSERT(mind);
        TS_ASSERT(MindList::GetFreeMind());
        mind->SetFree(false);
        TS_ASSERT(!MindList::GetFreeMind());
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

