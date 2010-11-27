#ifndef MIND_H
#define MIND_H

#include <Typedefs.h>

class Mind
{
public:
    Mind(UnitId aUnitId);
    void Update(GameTime aPeriod);
private:
    const UnitId mUnitId;
};

#endif // MIND_H
