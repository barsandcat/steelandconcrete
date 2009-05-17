#include <Network.h>

#include <stdexcept>
#include <Header.pb.h>

const int MESSAGE_SIZE = 1024;
const int HEADER_SIZE = 8;

std::string GetErrorText(socket_t& aSocket)
{
    char buffer[512];
    aSocket.get_error_text(buffer, 512);
    return buffer;
}

void WriteMessage(socket_t& aSocket, google::protobuf::Message& aMessage)
{
    int messageSize = aMessage.ByteSize();
    assert(messageSize <= MESSAGE_SIZE);
    char messageBuffer[MESSAGE_SIZE];
    aMessage.SerializeToArray(messageBuffer, messageSize);

    HeaderMsg header;
    header.set_size(messageSize);
    int headerSize = header.ByteSize();
    char headerBuffer[HEADER_SIZE];
    header.SerializeToArray(headerBuffer, headerSize);

    if (!aSocket.write(headerBuffer, headerSize))
        throw std::runtime_error(GetErrorText(aSocket) + " Неудалось записать в сокет загловок!");
    if (!aSocket.write(messageBuffer, messageSize))
        throw std::runtime_error(GetErrorText(aSocket) + " Неудалось записать в сокет сообщение!");
}

void ReadMessage(socket_t& aSocket, google::protobuf::Message& aMessage)
{
    HeaderMsg header;
    header.set_size(0);
    int headerSize = header.ByteSize();
    char headerBuffer[HEADER_SIZE];
    if (!aSocket.read(headerBuffer, headerSize))
        throw std::runtime_error(GetErrorText(aSocket) + " Не удалось прочитать из сокета заголовок!");
    if (!header.ParseFromArray(headerBuffer, headerSize))
        throw std::runtime_error("Не удалось разобрать заголовок!");

    int messageSize = header.size();
    assert(messageSize <= MESSAGE_SIZE);
    char messageBuffer[MESSAGE_SIZE];
    if (!aSocket.read(messageBuffer, messageSize))
        throw std::runtime_error(GetErrorText(aSocket) + " Не удалось прочитать из сокета сообщение!");
    if (!aMessage.ParseFromArray(messageBuffer, messageSize))
        throw std::runtime_error("Не удалось разобрать сообщение!");
}
