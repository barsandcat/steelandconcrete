#include <pch.h>
#include <Network.h>

#include <Header.pb.h>

const int HEADER_BUFFER_SIZE = 8;


Network::Network(SocketSharedPtr aSocket): mSocket(aSocket), mMessageBuffer(NULL), mBufferSize(0)
{
    assert(aSocket);
}

Network::~Network()
{
    delete mMessageBuffer;
    mSocket->close();
}

void Network::AllocBuffer(int aSize)
{
    if (aSize > mBufferSize)
    {
        delete mMessageBuffer;
        mMessageBuffer = new char[aSize];
    }
}

void Network::WriteMessage(const google::protobuf::Message& aMessage)
{
    int messageSize = aMessage.ByteSize();
    AllocBuffer(messageSize);
    aMessage.SerializeToArray(mMessageBuffer, messageSize);

    HeaderMsg header;
    header.set_size(messageSize);
    int headerSize = header.ByteSize();
    char headerBuffer[HEADER_BUFFER_SIZE];
    header.SerializeToArray(headerBuffer, headerSize);

    if (!mSocket->send(boost::asio::buffer(headerBuffer, headerSize)))
    {
        throw std::runtime_error("Неудалось записать в сокет загловок!");
    }
    if (!mSocket->send(boost::asio::buffer(mMessageBuffer, messageSize)))
    {
        throw std::runtime_error("Неудалось записать в сокет сообщение!");
    }
}

void Network::ReadMessage(google::protobuf::Message& aMessage)
{
    HeaderMsg header;
    header.set_size(0);
    int headerSize = header.ByteSize();
    char headerBuffer[HEADER_BUFFER_SIZE];
    if (!mSocket->receive(boost::asio::buffer(headerBuffer, headerSize)))
        throw std::runtime_error("Не удалось прочитать из сокета заголовок!");
    if (!header.ParseFromArray(headerBuffer, headerSize))
        throw std::runtime_error("Не удалось разобрать заголовок!");

    int messageSize = header.size();
    AllocBuffer(messageSize);

    if (!mSocket->receive(boost::asio::buffer(mMessageBuffer, messageSize)))
    {
        throw std::runtime_error("Не удалось прочитать из сокета сообщение!");
    }
    if (!aMessage.ParseFromArray(mMessageBuffer, messageSize))
    {
        throw std::runtime_error("Не удалось разобрать сообщение!");
    }
}
