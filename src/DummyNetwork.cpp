#include <pch.h>
#include <DummyNetwork.h>

#include <google/protobuf/descriptor.h>
#include <Response.pb.h>


void DummyNetwork::WriteMessage(const google::protobuf::Message& aMessage)
{
    const google::protobuf::Reflection* reflection = aMessage.GetReflection();
    const google::protobuf::FieldDescriptor* fieldDescriptor = aMessage.GetDescriptor()->FindFieldByName("type");
    if (fieldDescriptor)
    {
        mIsLastWrited = reflection->GetEnum(aMessage, fieldDescriptor)->number() == RESPONSE_OK;
    }

    fieldDescriptor = aMessage.GetDescriptor()->FindFieldByName("changes");
    if (fieldDescriptor)
    {
        mChangesWrited += reflection->FieldSize(aMessage, fieldDescriptor);
    }
}

void DummyNetwork::ReadMessage(google::protobuf::Message& aMessage)
{

}

bool DummyNetwork::IsOk()
{
    return true;
}

DummyNetwork::DummyNetwork(): mIsLastWrited(false), mChangesWrited(0)
{
    //ctor
}

DummyNetwork::~DummyNetwork()
{
    //dtor
}

DummyNetwork::DummyNetwork(const DummyNetwork& other)
{
    //copy ctor
}
