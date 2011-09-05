#include <pch.h>
#include <DummyNetwork.h>

#include <google/protobuf/descriptor.h>
#include <Payload.pb.h>


void DummyNetwork::WriteMessage(const google::protobuf::Message& aMessage)
{
    ++mWrites;
    const google::protobuf::Reflection* reflection = aMessage.GetReflection();
    const google::protobuf::FieldDescriptor* fieldDescriptor = aMessage.GetDescriptor()->FindFieldByName("last");
    if (fieldDescriptor)
    {
        mIsLastWrited = reflection->GetBool(aMessage, fieldDescriptor);
        if (mIsLastWrited)
        {
            fieldDescriptor = aMessage.GetDescriptor()->FindFieldByName("time");
            if (fieldDescriptor)
            {
                mTimeWrited = reflection->GetUInt64(aMessage, fieldDescriptor);
            }
        }
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

DummyNetwork::DummyNetwork(): mIsLastWrited(false), mChangesWrited(0), mWrites(0)
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
