#ifndef TILE_H
#define TILE_H
#include <Ogre.h>

class ClientTile
{
public:
    explicit ClientTile(const Ogre::Vector3& aPosition);
    ~ClientTile();
    void AddNeighbour(ClientTile* aTile) { mNeighbourhood.push_back(aTile); }
    void RemoveNeighbour(ClientTile* aTile);
    void SortNeighbourhood();

    Ogre::MeshPtr ConstructMesh(const Ogre::String& aMeshName) const;
    inline const Ogre::Vector3& GetPosition() const { return mPosition; }
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ClientTile* GetNeighbour(size_t aIndex) const { return mNeighbourhood[aIndex]; }

    size_t GetIndex() const { return mIndex; }
    void SetIndex(size_t aIndex) { mIndex = aIndex; }

    ClientTile* GetTileAtPosition(const Ogre::Vector3& aPosistion);
protected:
private:
    std::vector< ClientTile* > mNeighbourhood;
    const Ogre::Vector3 mPosition;
    size_t mIndex;
};

#endif // TILE_H
