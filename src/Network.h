#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <INetwork.h>
#include <boost/circular_buffer.hpp>

const size_t HEADER_BUFFER_SIZE = 8;

class Network: public INetwork
{
public:
    Network(SSLStreamPtr aSocket);
    ~Network();
    virtual void WriteMessage(const google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
private:
    void AllocBuffer(int aSize);
    SSLStreamPtr mSSLStream;
    char* mMessageBuffer;
    char mHeaderBuffer[HEADER_BUFFER_SIZE];
    size_t mHeaderSize;
    int mBufferSize;
};

#endif // NETWORK_H_INCLUDED
