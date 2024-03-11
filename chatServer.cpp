#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "chatServer.h"

ChatStat::ChatStat()
{
    serverSock = 0;
    numClients = 0;
    maxClient = -1;
    memset(clients, 0, sizeof(*clients));
}
void ChatStat::setServerSock(int serverSock_)
{
    serverSock = serverSock_;
}
int ChatStat::getServerSock()
{
    return serverSock;
}
void ChatStat::setNumMaxClient(int maxClient_)
{
    maxClient = maxClient_;
}
int ChatStat::getNumMaxClient()
{
    return maxClient;
}
Client** ChatStat::getClientsVec()
{
    return clients;
}

ChatServer::ChatServer() {
    chat = new ChatStat();
    memset(chat, 0, sizeof(*chat));
}
ChatServer::~ChatServer() {
    if (chat)
    {
        delete chat;
        chat = nullptr;
    }
}
void ChatServer::init()
{
    int s, yes = 1;
    sockaddr_in sa;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Creating listening socket");
        exit(1);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(SERVER_PORT);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (sockaddr*)&sa, sizeof(sa)) == -1 ||
        listen(s, 511) == -1)
    {
        perror("Creating listening socket");
        exit(1);
    }
    chat->setServerSock(s);
}
void ChatServer::start()
{
    coreFunc();
}
void ChatServer::coreFunc()
{
    while (true) {
        fd_set readFds;
        timeval tv;
        int retval;

        FD_ZERO(&readFds);
        FD_SET(chat->getServerSock(), &readFds);

        for (int j = 0; j <= chat->getNumMaxClient(), j++) {
            if (chat->)
        }
    }
}