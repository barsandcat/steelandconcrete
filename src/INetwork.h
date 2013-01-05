#ifndef INETWORK_H
#define INETWORK_H

#include <google/protobuf/message.h>
#include <boost/shared_ptr.hpp>

class INetwork
{
public:
    virtual void WriteMessage(const google::protobuf::Message& aMessage) = 0;
    virtual void ReadMessage(google::protobuf::Message& aMessage) = 0;
    virtual ~INetwork() {}
};

typedef boost::shared_ptr< INetwork > NetworkPtr;

#endif // INETWORK_H
