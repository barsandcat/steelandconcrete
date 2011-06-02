#ifndef ASYNCNETWORK_H_INCLUDED
#define ASYNCNETWORK_H_INCLUDED

#include <INetwork.h>
#include <google/protobuf/message.h>

class AsyncNetwork
{
public:
    AsyncNetwork(SocketSharedPtr aSocket);
    ~AsyncNetwork();
    virtual void WriteMessage(const google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
private:
    void AllocBuffer(int aSize);
    SocketSharedPtr mSocket;
    char* mMessageBuffer;
    int mBufferSize;
};

#endif // ASYNCNETWORK_H_INCLUDED
