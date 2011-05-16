#ifndef UNITLISTITERATOR_H
#define UNITLISTITERATOR_H

#include <vector>
#include <Typedefs.h>

class ServerUnit;

class UnitListIterator
{
public:
    UnitListIterator(std::vector< ServerUnit* >& aUnits);
    void Next();
    bool IsDone();
    ServerUnit* GetUnit() const { return mUnits[mIndex]; }
private:
    size_t mIndex;
    std::vector< ServerUnit* >& mUnits;
};

#endif // UNITLISTITERATOR_H
