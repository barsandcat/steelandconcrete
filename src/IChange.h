#ifndef ICHANGE_H
#define ICHANGE_H

#include <ChangeList.pb.h>
#include <Typedefs.h>
#include <set>

class ChangeList;
typedef std::map<TileId, const ChangeList*> VisibleTiles;

class IChange
{
public:
    virtual void FillChangeMsg(ChangeMsg& aChange, VisibleTiles& aVisibleTiles) const = 0;
};

inline IChange* new_clone( const IChange& r )
{
    return 0;
}

inline IChange* new_clone( const IChange* r )
{
    return 0;
}

inline void delete_clone( const IChange* r )
{
    delete r;
}
#endif // ICHANGE_H
