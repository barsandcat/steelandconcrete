#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <Response.pb.h>
#include <INetwork.h>
#include <boost/thread.hpp>

class ChangeList: public boost::noncopyable
{
public:
    typedef std::vector< ResponseMsg* > ResponseList;
    typedef std::pair<GameTime, ResponseList> UpdateBlock;

    void AddMove(UnitId aUnit, TileId aPosition);
    void AddCommandDone(UnitId aUnit);
    void Write(INetwork& aNetwork, GameTime aClientTime, int32 aUpdateLength);
    void Commit(GameTime aTime);
    void AddRemove(UnitId aUnit);
    void Clear();
private:
    typedef std::deque< UpdateBlock > UpdateBlockList;
	ChangeMsg& AddChangeMsg();
    UpdateBlockList mChangeList;
    ResponseList mCurrentChanges;
    boost::shared_mutex mChangeListRWL;
};

#endif // CHANGELIST_H
