#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <Typedefs.h>

class ServerGame;
class ServerUnit;

void ClientConnection(ServerGame& aGame, SSLStreamPtr aSocket);

#endif // CLIENTCONNECTION_H
