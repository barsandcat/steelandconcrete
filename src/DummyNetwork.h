#ifndef DUMMYNETWORK_H
#define DUMMYNETWORK_H

#include <INetwork.h>
class DummyNetwork: public INetwork
{
public:
    DummyNetwork();
    ~DummyNetwork();
    DummyNetwork(const DummyNetwork& other);
    virtual void WriteMessage(const google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
    virtual bool IsOk();
    bool IsLastWrited() const { return mIsLastWrited; }
    int GetChangesWrited() const { return mChangesWrited; }
    int GetWrites() const { return mWrites; }
protected:
private:
   bool mIsLastWrited;
   int mChangesWrited;
   int mWrites;
};

#endif // DUMMYNETWORK_H
