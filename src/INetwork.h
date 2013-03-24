#ifndef INETWORK_H
#define INETWORK_H

#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>
#include <Payload.pb.h>

class INetwork
{
public:
    virtual void WriteMessage(const PayloadMsg& aMessage) = 0;
    virtual void ReadMessage(PayloadMsg& aMessage) = 0;
    virtual ~INetwork() {}
};

typedef boost::shared_ptr< INetwork > NetworkPtr;

#endif // INETWORK_H
