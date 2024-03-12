#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <iostream>
#include "chatServer.h"

void Client::setFd(int fd_)
{
    fd = fd_;
}
int Client::getFd()
{
    return fd;
}
void Client::setNick(const std::string& nick_)
{
    nick = nick_;
}
const std::string& Client::getNick()
{
    return nick;
}

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
int ChatStat::acceptClient()
{
    int s;

    while (true) {
        sockaddr_in sa;
        socklen_t slen = sizeof(sa);
        s = accept(serverSock, (sockaddr*)&sa, &slen);
        if (s == -1) {
            if (errno == EINTR)
                continue;
            else   
                return -1;
        }
        break;
    }
    return s;
}
Client* ChatStat::createClient(int fd)
{
    char nick[32];
    int nicklen = snprintf(nick, sizeof(nick), "user:%d", fd);
    Client* c = new Client();
    socketSetNonBlockNoDelay(fd);
    c->setFd(fd);
    c->setNick(std::string(nick, nicklen));
    assert(clients[c->getFd()] == NULL);
    clients[c->getFd()] = c;
    if (c->getFd() > maxClient) maxClient = c->getFd();
    return c;
}
int ChatStat::socketSetNonBlockNoDelay(int fd)
{
    int flags, yes = 1;

    /* Set the socket nonblocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;

    /* This is best-effort. No need to check for errors. */
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    return 0;
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

        for (int j = 0; j <= chat->getNumMaxClient(); j++) {
            if (chat->getClientsVec()[j]) FD_SET(j, &readFds);
        }

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int maxFd = chat->getNumMaxClient();
        if (maxFd < chat->getServerSock()) maxFd = chat->getServerSock();
        retval = select(maxFd+1, &readFds, nullptr, nullptr, &tv);
        if (retval == -1) {
            perror("select() error");
            exit(1);
        } else if (retval) {
            if (FD_ISSET(chat->getServerSock(), &readFds)) {
                int fd = chat->acceptClient();
                Client* c =chat->createClient(fd);

                char* welcome_msg = 
                    "Welcome to Simple Chat! "
                    "Use /nick <nick> to set your nick.\n";
                write(c->getFd(), welcome_msg, strlen(welcome_msg));
                std::cout << "Connected client fd=" << c->getFd() << std::endl;
            }
        }
    }
}