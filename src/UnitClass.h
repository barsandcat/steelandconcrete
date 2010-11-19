#ifndef UNITCLASS_H
#define UNITCLASS_H

#include <Typedefs.h>

class UnitClass
{
public:
    UnitClass(uint32 aVisualCode, uint32 aMaxAge, uint32 aMaxSpeed);
    uint32 GetVisualCode() const { return mVisualCode; }
    uint32 GetMaxAge() const { return mMaxAge; }
    uint32 GetMaxSpeed() const { return mMaxSpeed; }
private:
    uint32 mVisualCode;
    uint32 mMaxAge;
    uint32 mMaxSpeed;
};

#endif // UNITCLASS_H
