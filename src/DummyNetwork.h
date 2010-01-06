#ifndef DUMMYNETWORK_H
#define DUMMYNETWORK_H

#include <INetwork.h>
class DummyNetwork: public INetwork
{
public:
    DummyNetwork();
    ~DummyNetwork();
    DummyNetwork(const DummyNetwork& other);
    virtual void WriteMessage(google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
    virtual bool IsOk();
    bool IsLastWrited() const { return mIsLastWrited; }
protected:
private:
   bool mIsLastWrited;
};

#endif // DUMMYNETWORK_H
