#include <pch.h>
#include <Network.h>

#include <Header.pb.h>

Network::Network(SSLStreamPtr aSSLStream): mSSLStream(aSSLStream), mMessageBuffer(NULL),
mBufferSize(0)
{
    HeaderMsg header;
    header.set_size(0);
    mHeaderSize = header.ByteSize();
}

Network::~Network()
{
    try
    {
        delete mMessageBuffer;
        mSSLStream->shutdown();
    }
    catch(std::exception& e)
    {
        std::cerr << "~Network exception: " << e.what();
    }
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
    //std::cout << "NET:WriteMessage " << aMessage.ShortDebugString() << std::endl;
    size_t messageSize = aMessage.ByteSize();
    AllocBuffer(messageSize);
    aMessage.SerializeToArray(mMessageBuffer, messageSize);

    HeaderMsg header;
    header.set_size(messageSize);
    size_t headerSize = header.ByteSize();
    header.SerializeToArray(mHeaderBuffer, headerSize);

    if (boost::asio::write(*mSSLStream, boost::asio::buffer(mHeaderBuffer, headerSize)) != headerSize)
    {
        boost::throw_exception(std::runtime_error("Неудалось записать в сокет загловок!"));
    }
    if (boost::asio::write(*mSSLStream, boost::asio::buffer(mMessageBuffer, messageSize)) != messageSize)
    {
        boost::throw_exception(std::runtime_error("Неудалось записать в сокет сообщение!"));
    }
}

void Network::ReadMessage(google::protobuf::Message& aMessage)
{
    if (boost::asio::read(*mSSLStream, boost::asio::buffer(mHeaderBuffer, mHeaderSize)) != mHeaderSize)
    {
        boost::throw_exception(std::runtime_error("Не удалось прочитать из сокета заголовок!"));
    }
    HeaderMsg header;
    if (!header.ParseFromArray(mHeaderBuffer, mHeaderSize))
    {
        boost::throw_exception(std::runtime_error("Не удалось разобрать заголовок!"));
    }

    size_t messageSize = header.size();
    AllocBuffer(messageSize);

    if (boost::asio::read(*mSSLStream, boost::asio::buffer(mMessageBuffer, messageSize)) != messageSize)
    {
        boost::throw_exception(std::runtime_error("Не удалось прочитать из сокета сообщение!"));
    }
    if (!aMessage.ParseFromArray(mMessageBuffer, messageSize))
    {
        boost::throw_exception(std::runtime_error("Не удалось разобрать сообщение!"));
    }

    //std::cout << "NET:ReadMessage " << aMessage.ShortDebugString() << std::endl;
}
