#ifndef TILE_H
#define TILE_H
#include <Ogre.h>

class ClientUnit;
class ClientGridNode;

class ClientTile
{
public:
    explicit ClientTile(bool ground, const ClientGridNode& aGridNode);
    ~ClientTile();

    Ogre::MeshPtr ConstructMesh(const Ogre::String& aMeshName) const;
    Ogre::SceneNode& GetNode() { return mNode; }

    ClientUnit* GetUnit() const { return mUnit; }
    void SetUnit(ClientUnit* aUnit) { mUnit = aUnit; }
protected:
private:
    Ogre::SceneNode& mNode;
    const bool mGround;
    ClientUnit* mUnit;
    const ClientGridNode& mGridNode;
};

#endif // TILE_H
