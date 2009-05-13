#ifndef SERVERTILE_H
#define SERVERTILE_H
#include <OgreVector3.h>

class ServerUnit;

class ServerTile
{
public:
    explicit ServerTile(const Ogre::Vector3& aPosition);
    ~ServerTile();
    void AddNeighbour(ServerTile& aTile) { mNeighbourhood.push_back(&aTile); }
    void RemoveNeighbour(ServerTile& aTile);
    void SortNeighbourhood();

    inline const Ogre::Vector3& GetPosition() const { return mPosition; }
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ServerTile& GetNeighbour(size_t aIndex) const { return *mNeighbourhood[aIndex]; }
    ServerUnit* GetUnit() const { return mUnit; }
    void TransferUnit(ServerTile& aTile);

    size_t GetIndex() const { return mIndex; }
    void SetIndex(size_t aIndex) { mIndex = aIndex; }

    ServerTile* GetTileAtPosition(const Ogre::Vector3& aPosistion);
protected:
private:
    std::vector< ServerTile* > mNeighbourhood;
    const Ogre::Vector3 mPosition;
    size_t mIndex;
    ServerUnit* mUnit;
};

#endif // SERVERTILE_H
