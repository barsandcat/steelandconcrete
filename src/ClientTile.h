#ifndef TILE_H
#define TILE_H
#include <Ogre.h>

class ClientUnit;
class ClientGridNode;

class ClientTile: public boost::noncopyable
{
public:
    explicit ClientTile(bool ground, ClientGridNode& aGridNode);
    ~ClientTile();

    Ogre::MeshPtr ConstructMesh(const Ogre::String& aMeshName) const;
    Ogre::SceneNode& GetNode() { return mNode; }
    ClientGridNode& GetGridNode() { return mGridNode; }

    ClientUnit* GetUnit() const { return mUnit; }
    void SetUnit(ClientUnit* aUnit) { mUnit = aUnit; }
private:
    Ogre::SceneNode& mNode;
    const bool mGround;
    ClientUnit* mUnit;
    ClientGridNode& mGridNode;
};

#endif // TILE_H
