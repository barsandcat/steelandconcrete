#ifndef INETWORK_H
#define INETWORK_H

#include <Payload.pb.h>
#include <boost/function.hpp>
typedef boost::shared_ptr< PayloadMsg > PayloadPtr;
typedef boost::shared_ptr< const PayloadMsg > ConstPayloadPtr;
typedef boost::function< void (ConstPayloadPtr) > ResponseCallBack;

class INetwork
{
public:
    virtual void WriteMessage(const google::protobuf::Message& aMessage) = 0;
    virtual void ReadMessage(google::protobuf::Message& aMessage) = 0;
    virtual void Request(ResponseCallBack aCallBack, ConstPayloadPtr aPayloadMsg) = 0;
    virtual ~INetwork() {}
};

typedef boost::shared_ptr< INetwork > NetworkPtr;

#endif // INETWORK_H
