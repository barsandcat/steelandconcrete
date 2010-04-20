#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <Response.pb.h>
#include <INetwork.h>

class ChangeList
{
public:
    typedef std::vector< ResponseMsg* > ResponseList;
    typedef std::pair<GameTime, ResponseList> UpdateBlock;

    static void AddMove(UnitId aUnit, TileId aPosition);
    static void AddCommandDone(UnitId aUnit);
    static void Write(INetwork& aNetwork, GameTime aClientTime);
    static void Commit(GameTime aTime);
    static void AddRemove(UnitId aUnit);
    static void Clear();
private:
    typedef std::deque< UpdateBlock > UpdateBlockList;
    static UpdateBlockList mChangeList;
    static ResponseList mCurrentChanges;
    static ChangeMsg& AddChangeMsg();
};

#endif // CHANGELIST_H
