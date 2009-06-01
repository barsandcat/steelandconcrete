#ifndef CHANGELIST_H
#define CHANGELIST_H
#include <Typedefs.h>
#include <ChangeList.pb.h>
#include <sockio.h>

class ChangeList
{
public:
    static void AddMove(UnitId aUnit, TileId aPosition);
    static void Clear();
    static void Write(socket_t& aSocket);
private:
    static ChangeListMsg mChangeList;
};

#endif // CHANGELIST_H
