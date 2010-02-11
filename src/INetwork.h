#ifndef INETWORK_H
#define INETWORK_H

#include <google/protobuf/message.h>

class INetwork
{
public:
    virtual void WriteMessage(const google::protobuf::Message& aMessage) = 0;
    virtual void ReadMessage(google::protobuf::Message& aMessage) = 0;
    virtual bool IsOk() = 0;
};

#endif // INETWORK_H
