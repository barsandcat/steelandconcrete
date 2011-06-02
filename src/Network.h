#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <INetwork.h>
#include <google/protobuf/message.h>
#include <Response.pb.h>
#include <Request.pb.h>
#include <boost/function.hpp>

typedef boost::shared_ptr< ResponseMsg > ResponsePtr;
typedef boost::function< void (ResponsePtr) > ReadCallBack;

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
    SocketSharedPtr mSocket;
    char* mMessageBuffer;
    int mBufferSize;
};

#endif // NETWORK_H_INCLUDED
