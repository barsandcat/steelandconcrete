#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

class ServerGame;
class ServerUnit;

void ClientConnection(ServerGame& aGame, SocketSharedPtr aSocket);

#endif // CLIENTCONNECTION_H
