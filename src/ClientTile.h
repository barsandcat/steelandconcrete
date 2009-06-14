#ifndef TILE_H
#define TILE_H
#include <Ogre.h>

class ClientUnit;

class ClientTile
{
public:
    explicit ClientTile(TileId aId, const Ogre::Vector3& aPosition);
    ~ClientTile();
    void AddNeighbour(ClientTile* aTile) { mNeighbourhood.push_back(aTile); }
    void RemoveNeighbour(ClientTile* aTile);
    void SortNeighbourhood();

    Ogre::MeshPtr ConstructMesh(const Ogre::String& aMeshName) const;
    inline const Ogre::Vector3& GetPosition() const { return mNode.getPosition(); }
    Ogre::SceneNode& GetNode() { return mNode; }
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ClientTile* GetNeighbour(size_t aIndex) const { return mNeighbourhood[aIndex]; }

    TileId GetTileId() const { return mTileId; }

    ClientTile* GetTileAtPosition(const Ogre::Vector3& aPosistion);
    ClientUnit* GetUnit() const { return mUnit; }
    void SetUnit(ClientUnit* aUnit) { mUnit = aUnit; }
protected:
private:
    std::vector< ClientTile* > mNeighbourhood;
    Ogre::SceneNode& mNode;
    TileId mTileId;
    ClientUnit* mUnit;
};

#endif // TILE_H
