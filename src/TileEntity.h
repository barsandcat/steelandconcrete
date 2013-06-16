#ifndef TILE_H
#define TILE_H
#include <Ogre.h>

class ClientUnit;
class ClientGridNode;

class TileEntity: public boost::noncopyable
{
public:
    explicit TileEntity(bool ground, ClientGridNode& aGridNode);
    ~TileEntity();

    Ogre::SceneNode& GetNode() { return *mNode; }
    Ogre::Vector3 GetPosition() const;

private:
    Ogre::MeshPtr ConstructMesh(const Ogre::String& aMeshName) const;
    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;
    const bool mGround;
    ClientGridNode& mGridNode;
};

#endif // TILE_H
