#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <sockio.h>
#include <google/protobuf/message.h>

class Network
{
public:
    Network(socket_t* aSocket);
    ~Network();
    void WriteMessage(google::protobuf::Message& aMessage);
    void ReadMessage(google::protobuf::Message& aMessage);
    bool IsOk() { return mSocket->is_ok(); }
private:
    void AllocBuffer(int aSize);
    socket_t* mSocket;
    char* mMessageBuffer;
    int mBufferSize;
};

const int MESSAGE_SIZE = 1200;

std::string GetErrorText(socket_t& aSocket);

#endif // NETWORK_H_INCLUDED
