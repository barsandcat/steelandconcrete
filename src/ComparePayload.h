#ifndef COMPAREPAYLOAD_H_INCLUDED
#define COMPAREPAYLOAD_H_INCLUDED

#include <Payload.pb.h>

bool operator==(const google::protobuf::Message& aMsg1, const google::protobuf::Message& aMsg2);

#endif // COMPAREPAYLOAD_H_INCLUDED
