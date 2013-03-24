#ifndef SERVER_PROXY_H_INCLUDED
#define SERVER_PROXY_H_INCLUDED

#include <google/protobuf/message.h>
#include <boost/circular_buffer.hpp>
#include <Payload.pb.h>

typedef boost::shared_ptr< PayloadMsg > PayloadPtr;
typedef boost::shared_ptr< const PayloadMsg > ConstPayloadPtr;
typedef boost::function< void (ConstPayloadPtr) > ResponseCallBack;

typedef boost::circular_buffer< std::pair<ResponseCallBack, PayloadPtr> > Requests;
const size_t HEADER_BUFFER_SIZE = 8;

class IServerProxy
{
public:
    virtual void Request(ResponseCallBack aCallBack, PayloadPtr aPayloadMsg) = 0;
};

class ServerProxy: public IServerProxy
{
public:
    ServerProxy(SSLStreamPtr aSSLStream);
    ~ServerProxy();
    virtual void Request(ResponseCallBack aCallBack, PayloadPtr aPayloadMsg);
    int32 GetInBytes() const { return mInBytes; }
    int32 GetOutBytes() const { return mOutBytes; }
    int32 GetPing() const { return mPing; }
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
    SSLStreamPtr mSSLStream;
    char* mMessageBuffer;
    char mHeaderBuffer[HEADER_BUFFER_SIZE];
    size_t mHeaderSize;
    int mBufferSize;
    bool mAsync;
    Requests mRequests;
    int32 mInBytes;
    int32 mOutBytes;
    int64 mRequestTime;
    int64 mPing;
};

typedef boost::shared_ptr< ServerProxy > ServerProxyPtr;


#endif
