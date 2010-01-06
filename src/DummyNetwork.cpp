#include <pch.h>
#include <DummyNetwork.h>

#include <google/protobuf/descriptor.h>


void DummyNetwork::WriteMessage(google::protobuf::Message& aMessage)
{
    const google::protobuf::FieldDescriptor* fieldDescriptor = aMessage.GetDescriptor()->FindFieldByName("last");
    if (fieldDescriptor)
    {
        const google::protobuf::Reflection* reflection = aMessage.GetReflection();
        mIsLastWrited = reflection->GetBool(aMessage, fieldDescriptor);
    }
}

void DummyNetwork::ReadMessage(google::protobuf::Message& aMessage)
{

}

bool DummyNetwork::IsOk()
{
    return true;
}

DummyNetwork::DummyNetwork(): mIsLastWrited(false)
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
