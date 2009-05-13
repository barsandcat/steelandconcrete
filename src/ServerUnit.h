#ifndef SERVERUNIT_H
#define SERVERUNIT_H

class ServerTile;

class ServerUnit
{
public:
    ServerUnit(ServerTile& aTile, int aIndex);
    void SetTile(ServerTile& aTile) { mTile = &aTile; }
    ServerTile& GetTile() const { return *mTile; }
    int GetIndex() const { return mIndex; }
    virtual ~ServerUnit();
    void Update();
protected:
private:
    ServerTile* mTile;
    int mIndex;
};

#endif // SERVERUNIT_H
