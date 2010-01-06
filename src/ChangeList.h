#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <Response.pb.h>
#include <INetwork.h>

class ChangeList
{
public:
    static void AddMove(UnitId aUnit, TileId aPosition);
    static void AddCommandDone(UnitId aUnit);
    static void Clear();
    static void Write(INetwork& aNetwork, GameTime aTime);
    static void AddRemove(UnitId aUnit);
private:
    static std::list< ResponseMsg > mChangeList;
    static void AddChangeMsg(ChangeMsg& aChange);
};

#endif // CHANGELIST_H
