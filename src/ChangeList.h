#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <Response.pb.h>
#include <INetwork.h>
#include <boost/circular_buffer.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <Change.h>


class ChangeList
{
public:
    typedef boost::ptr_vector<IChange> TurnChanges;
    ChangeList(): mChanges(100) { mChanges.push_front(TurnChanges()); }
    void AddMove(UnitId aUnit, TileId aPosition);
    void AddCommandDone(UnitId aUnit);
    void AddRemove(UnitId aUnit);
    void Write(INetwork& aNetwork, size_t aIndex) const;
    void Commit();
    void Clear();
private:
    boost::circular_buffer<TurnChanges> mChanges;
};

#endif // CHANGELIST_H
