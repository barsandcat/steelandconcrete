#ifndef TRACER_H_INCLUDED
#define TRACER_H_INCLUDED
#include <Typedefs.h>
#include <Change.h>

template <typename Type, int id>
class Tracer
{
public:
    Tracer(Type aValue, ChangeList& aChangeList):
      mChangeList(aChangeList), mValue(aValue)
    {
    }

    operator Type() const
    {
        return mValue;
    }

    Tracer& operator=(Type& aValue)
    {
        mValue = aValue;
        mChangeList.push_back(new Change<Type, id>(aValue));
        return *this;
    }

    int GetId() const
    {
        return id;
    }
private:
    ChangeList& mChangeList;
    Type mValue;
};

#endif // TRACER_H_INCLUDED
