#include <pch.h>
#include <ServerProxy.h>

#include <Header.pb.h>

#include <HighResolutionClock.h>

ServerProxy::ServerProxy(SSLStreamPtr aSSLStream): mSSLStream(aSSLStream), mMessageBuffer(NULL),
mBufferSize(0), mAsync(false), mRequests(100), mInBytes(0), mOutBytes(0), mRequestTime(0), mPing(0)
{
    HeaderMsg header;
    header.set_size(0);
    mHeaderSize = header.ByteSize();
}

ServerProxy::~ServerProxy()
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

void ServerProxy::AllocBuffer(int aSize)
{
    if (aSize > mBufferSize)
    {
        delete mMessageBuffer;
        mMessageBuffer = new char[aSize];
        mBufferSize = aSize;
    }
}

void ServerProxy::Request(ResponseCallBack aCallBack, PayloadPtr aPayloadMsg)
{

    if (mAsync)
    {
        mRequests.push_back(std::make_pair(aCallBack, aPayloadMsg));
    }
    else
    {
        mAsync = true;
        WriteRequest(aCallBack, aPayloadMsg);
    }
}

void ServerProxy::WriteRequest(ResponseCallBack aCallBack, PayloadPtr aPayloadMsg)
{
    mRequestTime = GetMiliseconds();
    //std::cout << "NET:WriteRequest " << aPayloadMsg->ShortDebugString() << std::endl;
    size_t messageSize = aPayloadMsg->ByteSize();
    HeaderMsg header;
    header.set_size(messageSize);
    size_t headerSize = header.ByteSize();
    header.SerializeToArray(mHeaderBuffer, headerSize);
    mOutBytes += headerSize;
    mOutBytes += messageSize;
    AllocBuffer(messageSize);
    aPayloadMsg->SerializeToArray(mMessageBuffer, messageSize);


    boost::array<boost::asio::mutable_buffer, 2> bufs = {{
        boost::asio::buffer(mHeaderBuffer, headerSize),
        boost::asio::buffer(mMessageBuffer, messageSize)}};

    boost::asio::async_write(*mSSLStream, bufs,
                             boost::bind(&ServerProxy::ReadResponse,
                                         this, aCallBack,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void ServerProxy::ReadResponse(ResponseCallBack aCallBack,
                           const boost::system::error_code& aError,
                           std::size_t aBytesTransferred)
{
    if (aError)
    {
        boost::throw_exception(std::runtime_error("Не удалось отправить сообщение!"));
    }

    boost::asio::async_read(*mSSLStream, boost::asio::buffer(mHeaderBuffer, mHeaderSize),
                            boost::bind(&ServerProxy::ParseHeader,
                                        this, aCallBack,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void ServerProxy::ParseHeader(ResponseCallBack aCallBack,
                          const boost::system::error_code& aError,
                          std::size_t aBytesTransferred)
{
    mPing = GetMiliseconds() - mRequestTime;

    //std::cout << "NET:ParseHeader " << aError << " " << aBytesTransferred << std::endl;
    if (aError)
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

    mInBytes += messageSize;

    boost::asio::async_read(*mSSLStream, boost::asio::buffer(mMessageBuffer, messageSize),
                            boost::bind(&ServerProxy::ParseMessage,
                                        this, aCallBack,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void ServerProxy::ParseMessage(ResponseCallBack aCallBack,
                           const boost::system::error_code& aError,
                           std::size_t aBytesTransferred)
{
    //std::cout << "NET:ParseMessage " << aError << " " << aBytesTransferred << std::endl;
    if (aError)
    {
        boost::throw_exception(std::runtime_error("Не удалось прочитать из сокета сообщение!"));
    }

    mInBytes += aBytesTransferred;

    boost::shared_ptr<PayloadMsg> msg(new PayloadMsg());
    if (!msg->ParseFromArray(mMessageBuffer, aBytesTransferred))
    {
        boost::throw_exception(std::runtime_error("Не удалось разобрать сообщение!"));
    }

    //std::cout << "NET:ParseMessage " << msg->ShortDebugString() << std::endl;
    aCallBack(msg);

    if (!msg->last())
    {
        ReadResponse(aCallBack, aError, aBytesTransferred);
    }
    else
    {
        if (!mRequests.empty())
        {
            std::pair<ResponseCallBack, PayloadPtr> nextRequest = mRequests.front();
            mRequests.pop_front();
            WriteRequest(nextRequest.first, nextRequest.second);
        }
        else
        {
            mAsync = false;
        }
    }
}
