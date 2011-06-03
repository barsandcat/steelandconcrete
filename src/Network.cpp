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
    mSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    mSocket->close();
}

void Network::AllocBuffer(int aSize)
{
    if (aSize > mBufferSize)
    {
        delete mMessageBuffer;
        mMessageBuffer = new char[aSize];
        mBufferSize = aSize;
    }
}

void Network::WriteMessage(const google::protobuf::Message& aMessage)
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

void Network::ReadMessage(google::protobuf::Message& aMessage)
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


void Network::AsynReadMessage(ReadCallBack aCallBack)
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

    boost::asio::async_read(*mSocket, boost::asio::buffer(mMessageBuffer, messageSize),
                            boost::bind(&Network::ParseMessage,
                                        this, aCallBack,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void Network::ParseMessage(ReadCallBack aCallBack,
                           const boost::system::error_code& aError,
                           std::size_t aBytesTransferred)
{
    if (aError)
    {
        boost::throw_exception(std::runtime_error("Не удалось прочитать из сокета сообщение!"));
    }

    boost::shared_ptr<ResponseMsg> msg(new ResponseMsg());
    if (!msg->ParseFromArray(mMessageBuffer, aBytesTransferred))
    {
        boost::throw_exception(std::runtime_error("Не удалось разобрать сообщение!"));
    }

    aCallBack(msg);
    if (msg->type() == RESPONSE_PART)
    {
        AsynReadMessage(aCallBack);
    }
}
