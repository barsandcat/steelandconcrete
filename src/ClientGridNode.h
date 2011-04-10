#ifndef CLIENTGRIDNODE_H
#define CLIENTGRIDNODE_H


class ClientUnit;

class ClientGridNode: public boost::noncopyable
{
public:
    explicit ClientGridNode(TileId aId, float scale, bool ground, const Ogre::Vector3& aPosition);
    ~ClientGridNode();
    void AddNeighbour(ClientGridNode* aTile) { mNeighbourhood.push_back(aTile); }
    void RemoveNeighbour(ClientGridNode* aTile);
    void SortNeighbourhood();

    Ogre::MeshPtr ConstructMesh(const Ogre::String& aMeshName) const;
    inline const Ogre::Vector3& GetPosition() const { return mNode.getPosition(); }
    Ogre::SceneNode& GetNode() { return mNode; }
    inline size_t GetNeighbourCount() const { return mNeighbourhood.size(); }
    ClientGridNode* GetNeighbour(size_t aIndex) const { return mNeighbourhood[aIndex]; }

    TileId GetTileId() const { return mTileId; }

    ClientGridNode* GetTileAtPosition(const Ogre::Vector3& aPosistion);
    ClientUnit* GetUnit() const { return mUnit; }
    void SetUnit(ClientUnit* aUnit) { mUnit = aUnit; }
private:
    std::vector< ClientGridNode* > mNeighbourhood;
    Ogre::SceneNode& mNode;
    TileId mTileId;
    const bool mGround;
    ClientUnit* mUnit;
};

#endif // CLIENTGRIDNODE_H

