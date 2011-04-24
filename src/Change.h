#ifndef CHANGE_H
#define CHANGE_H

#include <ChangeList.pb.h>

class IChange
{
public:
    virtual void FillChangeMsg(ChangeMsg& aChange) const = 0;
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
#endif // CHANGE_H
