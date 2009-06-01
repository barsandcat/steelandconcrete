#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <sockio.h>
#include <google/protobuf/message.h>

const int MESSAGE_SIZE = 1200;

void WriteMessage(socket_t& aSocket, google::protobuf::Message& aMessage);
void ReadMessage(socket_t& aSocket, google::protobuf::Message& aMessage);

#endif // NETWORK_H_INCLUDED
