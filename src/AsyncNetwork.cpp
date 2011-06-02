#include <pch.h>
#include <AsyncNetwork.h>

#include <Header.pb.h>

const int HEADER_BUFFER_SIZE = 8;


AsyncNetwork::AsyncNetwork(SocketSharedPtr aSocket): mSocket(aSocket), mMessageBuffer(NULL), mBufferSize(0)
{
    assert(aSocket);
}

AsyncNetwork::~AsyncNetwork()
{
    delete mMessageBuffer;
    mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    mSocket->close();
}

void AsyncNetwork::AllocBuffer(int aSize)
{
    if (aSize > mBufferSize)
    {
        delete mMessageBuffer;
        mMessageBuffer = new char[aSize];
    }
}

void AsyncNetwork::WriteMessage(const google::protobuf::Message& aMessage)
{
    size_t messageSize = aMessage.ByteSize();
    AllocBuffer(messageSize);
    aMessage.SerializeToArray(mMessageBuffer, messageSize);

    HeaderMsg header;
    header.set_size(messageSize);
    size_t headerSize = header.ByteSize();
    char headerBuffer[HEADER_BUFFER_SIZE];
    header.SerializeToArray(headerBuffer, headerSize);

    if (boost::asio::write(*mSocket, boost::asio::buffer(headerBuffer, headerSize)) != headerSize)
    {
        boost::throw_exception(std::runtime_error("Неудалось записать в сокет загловок!"));
    }
    if (boost::asio::write(*mSocket, boost::asio::buffer(mMessageBuffer, messageSize)) != messageSize)
    {
        boost::throw_exception(std::runtime_error("Неудалось записать в сокет сообщение!"));
    }
}

void AsyncNetwork::ReadMessage(google::protobuf::Message& aMessage)
{
    HeaderMsg header;
    header.set_size(0);
    size_t headerSize = header.ByteSize();
    char headerBuffer[HEADER_BUFFER_SIZE];
    if (boost::asio::read(*mSocket, boost::asio::buffer(headerBuffer, headerSize)) != headerSize)
    {
        boost::throw_exception(std::runtime_error("Не удалось прочитать из сокета заголовок!"));
    }
    if (!header.ParseFromArray(headerBuffer, headerSize))
    {
        boost::throw_exception(std::runtime_error("Не удалось разобрать заголовок!"));
    }

    size_t messageSize = header.size();
    AllocBuffer(messageSize);

    if (boost::asio::read(*mSocket, boost::asio::buffer(mMessageBuffer, messageSize)) != messageSize)
    {
        boost::throw_exception(std::runtime_error("Не удалось прочитать из сокета сообщение!"));
    }
    if (!aMessage.ParseFromArray(mMessageBuffer, messageSize))
    {
        boost::throw_exception(std::runtime_error("Не удалось разобрать сообщение!"));
    }
}
