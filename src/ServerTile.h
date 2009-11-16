#ifndef SERVERTILE_H
#define SERVERTILE_H
#include <Typedefs.h>
#include <OgreVector3.h>

class ServerUnit;

class ServerTile
{
public:
    explicit ServerTile(const Ogre::Vector3& aPosition, int32 aHeight);
    ~ServerTile();
    void AddNeighbour(ServerTile& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ServerTile& aTile);
    void SortNeighbourhood();

    const Ogre::Vector3& GetPosition() const { return mPosition; }
    size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ServerTile& GetNeighbour(size_t aIndex) const { return *mNeighbourhood[aIndex]; }
    ServerUnit* GetUnit() const { return mUnit; }
    void SetUnit(ServerUnit* aUnit) { mUnit = aUnit; }

    TileId GetTileId() const { return mTileId; }
    void SetTileId(TileId aTileId) { mTileId = aTileId; }
    int32 GetHeight() const { return mHeight; }

protected:
private:
    std::vector< ServerTile* > mNeighbourhood;
    const Ogre::Vector3 mPosition;
    const int32 mHeight;
    TileId mTileId;
    ServerUnit* mUnit;
};

#endif // SERVERTILE_H
