#ifndef AVATAR_H
#define AVATAR_H

#include <MindList.h>

class Avatar
{
public:
    Avatar(ServerGame& aGame): mId(MindList::GetAvatar()) { }
    ~Avatar() { MindList::NewMind(mId); }
    UnitId GetId() const { return mId; }
private:
    const UnitId mId;
};

#endif // AVATAR_H
