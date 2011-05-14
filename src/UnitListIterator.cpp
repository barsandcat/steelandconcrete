#include <pch.h>
#include <UnitListIterator.h>

UnitListIterator::UnitListIterator(std::vector< ServerUnit* >& aUnits): mUnits(aUnits), mIndex(0)
{
    while (mIndex < mUnits.size() && !mUnits[mIndex])
    {
        ++mIndex;
    }
}

void UnitListIterator::Next()
{
    do
    {
        ++mIndex;
    }
    while(mIndex < mUnits.size() && !mUnits[mIndex]);
}

bool UnitListIterator::IsDone()
{
    return mIndex >= mUnits.size();
}
