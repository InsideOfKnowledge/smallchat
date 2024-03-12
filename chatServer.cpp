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

Client::~Client() {
    if (_fd != -1)
        close(_fd);
}

void Client::setFd(int fd) {
    _fd = fd;
}

int Client::getFd() const {
    return _fd;
}

void Client::setNick(const std::string& nick) {
    _nick = nick;
}

const std::string& Client::getNick() const {
    return _nick;
}


ChatServer::ChatServer() 
    : _serverSock(-1), _numClients(0), _maxClient(-1) {
}

ChatServer::~ChatServer() {
    if (_serverSock != -1)
        close(_serverSock);

    for (auto& client : _clientVec) {
        delete client;
        client = nullptr;
    }
    _clientVec.clear();
}

ChatServer& ChatServer::getInstance() {
    static ChatServer instance;
    return instance; 
}

bool ChatServer::start() {
    if (!init())
        return false;

    while (true) {
        fd_set readFds;
        timeval tv;
        int retval;

        FD_ZERO(&readFds);
        FD_SET(_serverSock, &readFds);

        for (const auto& client : _clientVec) {
            FD_SET(client->getFd(), &readFds);
        }

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int maxFd = _serverSock > _maxClient ? _serverSock : _maxClient;
        retval = select(maxFd+1, &readFds, nullptr, nullptr, &tv);
        if (retval == -1) {
            perror("select() error");
            return false;
        } else if (retval) {
            if (FD_ISSET(_serverSock, &readFds)) {
                sendWelcomeToClient();
            }
        }
    }
}

bool ChatServer::init() {
    int s, yes = 1;
    sockaddr_in sa;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Creating listening socket");
        return false;
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
        return false;
    }
    _serverSock = s;
    
    return true;
}

int ChatServer::acceptClient() {
    int s;

    while (true) {
        sockaddr_in sa;
        socklen_t slen = sizeof(sa);
        s = accept(_serverSock, (sockaddr*)&sa, &slen);
        if (s == -1) {
            if (errno == EINTR)
                continue;
            else   
                return -1;
        }
        break;
    }

    char nick[32];
    int nicklen = snprintf(nick, sizeof(nick), "user:%d", s);
    Client* c = new Client();
    socketSetNonBlockNoDelay(s);
    c->setFd(s);
    c->setNick(std::string(nick, nicklen));
    
    _clientVec.push_back(c);
    _numClients++;
    _maxClient = s;
    
    return s;
}

void ChatServer::sendWelcomeToClient() {
    int fd = acceptClient();

    char* welcome_msg = 
        "Welcome to Simple Chat! "
        "Use /nick <nick> to set your nick.\n";
    write(fd, welcome_msg, strlen(welcome_msg));
    std::cout << "Connected client fd=" << fd << std::endl;
}

int ChatServer::socketSetNonBlockNoDelay(int fd)
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