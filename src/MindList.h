#ifndef MINDLIST_H
#define MINDLIST_H

#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <Typedefs.h>
class Mind;

class MindList
{
public:
    static void NewMind(UnitId aUnitId);
    static void UpdateMinds(GameTime aPeriod);
    static size_t GetSize() {  return mMinds.size(); }
    static Mind* GetFreeMind();
    static void Clear();
private:
    typedef boost::ptr_unordered_map<UnitId, Mind> MindListMap;
    static MindListMap mMinds;
};

#endif // MINDLIST_H
