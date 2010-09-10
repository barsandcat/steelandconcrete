#ifndef SERVERTILE_H
#define SERVERTILE_H
#include <Typedefs.h>
#include <OgreVector3.h>

class ServerTile: public boost::noncopyable
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
    UnitId GetUnitId() const { return mUnit; }
    void SetUnitId(UnitId aUnit) { mUnit = aUnit; }

    TileId GetTileId() const { return mTileId; }
    void SetTileId(TileId aTileId) { mTileId = aTileId; }
    int32 GetHeight() const { return mHeight; }

private:
    std::vector< ServerTile* > mNeighbourhood;
    const Ogre::Vector3 mPosition;
    const int32 mHeight;
    TileId mTileId;
    UnitId mUnit;
};

#endif // SERVERTILE_H
