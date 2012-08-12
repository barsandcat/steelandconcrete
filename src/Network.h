#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <INetwork.h>
#include <google/protobuf/message.h>
#include <boost/circular_buffer.hpp>

typedef boost::circular_buffer< std::pair<ResponseCallBack, PayloadPtr> > Requests;
const size_t HEADER_BUFFER_SIZE = 8;

class Network: public INetwork
{
public:
    Network(SocketSharedPtr aSocket);
    ~Network();
    virtual void WriteMessage(const google::protobuf::Message& aMessage);
    virtual void ReadMessage(google::protobuf::Message& aMessage);
    virtual void Request(ResponseCallBack aCallBack, PayloadPtr aPayloadMsg);
private:
    void AllocBuffer(int aSize);
    void WriteRequest(ResponseCallBack aCallBack, PayloadPtr aPayloadMsg);
    void ReadResponse(ResponseCallBack aCallBack,
                      const boost::system::error_code& aError,
                      std::size_t aBytesTransferred);
    void ParseHeader(ResponseCallBack aCallBack,
                     const boost::system::error_code& aError,
                     std::size_t aBytesTransferred);
    void ParseMessage(ResponseCallBack aCallBack,
                      const boost::system::error_code& aError,
                      std::size_t aBytesTransferred);
    SocketSharedPtr mSocket;
    char* mMessageBuffer;
    char mHeaderBuffer[HEADER_BUFFER_SIZE];
    size_t mHeaderSize;
    int mBufferSize;
    bool mAsync;
    Requests mRequests;
};

#endif // NETWORK_H_INCLUDED
