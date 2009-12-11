#include <pch.h>
#include <Network.h>

#include <Header.pb.h>

const int HEADER_BUFFER_SIZE = 8;

std::string GetErrorText(socket_t& aSocket)
{
    char buffer[512];
    aSocket.get_error_text(buffer, 512);
    return buffer;
}


Network::Network(socket_t* aSocket): mSocket(aSocket), mMessageBuffer(NULL), mBufferSize(0)
{
    assert(aSocket);
}

Network::~Network()
{
    delete mMessageBuffer;
    mSocket->close();
    delete mSocket;
}

void Network::AllocBuffer(int aSize)
{
    if (aSize > mBufferSize)
    {
        delete mMessageBuffer;
        mMessageBuffer = new char[aSize];
    }
}

void Network::WriteMessage(google::protobuf::Message& aMessage)
{
    int messageSize = aMessage.ByteSize();
    AllocBuffer(messageSize);
    aMessage.SerializeToArray(mMessageBuffer, messageSize);

    HeaderMsg header;
    header.set_size(messageSize);
    int headerSize = header.ByteSize();
    char headerBuffer[HEADER_BUFFER_SIZE];
    header.SerializeToArray(headerBuffer, headerSize);

    if (!mSocket->write(headerBuffer, headerSize))
    {
        throw std::runtime_error(GetErrorText(*mSocket) + " Неудалось записать в сокет загловок!");
    }
    if (!mSocket->write(mMessageBuffer, messageSize))
    {
        throw std::runtime_error(GetErrorText(*mSocket) + " Неудалось записать в сокет сообщение!");
    }
}

void Network::ReadMessage(google::protobuf::Message& aMessage)
{
    HeaderMsg header;
    header.set_size(0);
    int headerSize = header.ByteSize();
    char headerBuffer[HEADER_BUFFER_SIZE];
    if (!mSocket->read(headerBuffer, headerSize))
        throw std::runtime_error(GetErrorText(*mSocket) + " Не удалось прочитать из сокета заголовок!");
    if (!header.ParseFromArray(headerBuffer, headerSize))
        throw std::runtime_error("Не удалось разобрать заголовок!");

    int messageSize = header.size();
    AllocBuffer(messageSize);

    if (!mSocket->read(mMessageBuffer, messageSize))
    {
        throw std::runtime_error(GetErrorText(*mSocket) + " Не удалось прочитать из сокета сообщение!");
    }
    if (!aMessage.ParseFromArray(mMessageBuffer, messageSize))
    {
        throw std::runtime_error("Не удалось разобрать сообщение!");
    }
}
