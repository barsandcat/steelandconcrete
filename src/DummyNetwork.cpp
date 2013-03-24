#include <pch.h>
#include <DummyNetwork.h>

#include <google/protobuf/descriptor.h>
#include <Payload.pb.h>


void DummyNetwork::WriteMessage(const PayloadMsg& aMessage)
{
    mMessages.push_back(aMessage);
    ++mWrites;
    if (aMessage.has_last() && aMessage.last())
    {
        mIsLastWrited = true;
    }
    if (aMessage.has_time())
    {
        mTimeWrited = aMessage.time();
    }
    mChangesWrited += aMessage.changes_size();
}

void DummyNetwork::ReadMessage(PayloadMsg& aMessage)
{

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
