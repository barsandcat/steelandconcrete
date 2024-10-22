#ifndef DUMMYNETWORK_H
#define DUMMYNETWORK_H

#include <INetwork.h>
class DummyNetwork: public INetwork
{
public:
    DummyNetwork();
    ~DummyNetwork();
    DummyNetwork(const DummyNetwork& other);
    virtual void WriteMessage(const PayloadMsg& aMessage);
    virtual void ReadMessage(PayloadMsg& aMessage);

    bool IsLastWrited() const { return mIsLastWrited; }
    int GetChangesWrited() const { return mChangesWrited; }
    int GetWrites() const { return mWrites; }
    GameTime GetTimeWrited() const { return mTimeWrited; }
    const std::vector<PayloadMsg>& GetMessages() const { return mMessages; }
private:
   bool mIsLastWrited;
   int mChangesWrited;
   int mWrites;
   GameTime mTimeWrited;
   std::vector<PayloadMsg> mMessages;
};

#endif // DUMMYNETWORK_H
