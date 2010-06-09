#ifndef AVATAR_H
#define AVATAR_H

#include <ServerGame.h>

class Avatar
{
public:
    Avatar(ServerGame& aGame): mId(aGame.AssignAvatar()), mGame(aGame)
    {
    }

    ~Avatar()
    {
        mGame.FreeAvatar(mId);
    }

    UnitId GetId() const
    {
        return mId;
    }
protected:
private:
    const UnitId mId;
    ServerGame& mGame;
};

#endif // AVATAR_H
