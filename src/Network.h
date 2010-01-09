#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <INetwork.h>
#include <sockio.h>
#include <google/protobuf/message.h>

class Network: public INetwork
{
public:
    Network(socket_t* aSocket);
    ~Network();
    virtual void WriteMessage(google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
    virtual bool IsOk() { return mSocket->is_ok(); }
private:
    void AllocBuffer(int aSize);
    socket_t* mSocket;
    char* mMessageBuffer;
    int mBufferSize;
};

std::string GetErrorText(socket_t& aSocket);

#endif // NETWORK_H_INCLUDED
