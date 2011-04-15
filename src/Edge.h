#ifndef EDGE_H
#define EDGE_H

template <typename T>
class Edge: public boost::noncopyable
{
public:
    Edge(T& aTileA, T& aTileB): mTileA(aTileA), mTileB(aTileB)
    {
        mTileA.AddNeighbour(mTileB);
        mTileB.AddNeighbour(mTileA);
    }

    ~Edge()
    {
        mTileA.RemoveNeighbour(mTileB);
        mTileB.RemoveNeighbour(mTileA);
    }

    T& GetTileA() const { return mTileA; }
    T& GetTileB() const { return mTileB; }
private:
    T& mTileA;
    T& mTileB;
};

#endif // SERVEREDGE_H
