#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <INetwork.h>
#include <google/protobuf/message.h>
#include <Response.pb.h>
#include <Request.pb.h>
#include <boost/function.hpp>

typedef boost::shared_ptr< const ResponseMsg > ResponsePtr;
typedef boost::function< void (ResponsePtr) > ReadCallBack;
const size_t HEADER_BUFFER_SIZE = 8;

class Network: public INetwork
{
public:
    Network(SocketSharedPtr aSocket);
    ~Network();
    virtual void WriteMessage(const google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
    void AsynReadMessage(ReadCallBack aCallBack);
private:
    void AllocBuffer(int aSize);
    void ParseMessage(ReadCallBack aCallBack,
                      const boost::system::error_code& aError,
                      std::size_t aBytesTransferred);
    void ParseHeader(ReadCallBack aCallBack,
                     const boost::system::error_code& aError,
                     std::size_t aBytesTransferred);
    SocketSharedPtr mSocket;
    char* mMessageBuffer;
    char mHeaderBuffer[HEADER_BUFFER_SIZE];
    size_t mHeaderSize;
    int mBufferSize;
};

#endif // NETWORK_H_INCLUDED
