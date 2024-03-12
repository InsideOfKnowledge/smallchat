#include <string>

#define MAX_CLIENTS 1000
#define SERVER_PORT 7711

class Client
{
public:
    Client() = default;
    ~Client();
    void setFd(int fd_);
    int getFd();
    void setNick(const std::string& nick_);
    const std::string& getNick();

private:
    int fd;
    std::string nick;
};

class ChatStat
{
public:
    ChatStat();
    ~ChatStat();
    void setServerSock(int serverSock_);
    int getServerSock();
    void setNumMaxClient(int maxClient_);
    int getNumMaxClient();
    Client** getClientsVec();
    int acceptClient();
    void freeClient();
    Client* createClient(int fd);
    int socketSetNonBlockNoDelay(int fd);

private:
    int serverSock;
    int numClients;
    int maxClient;
    Client* clients[MAX_CLIENTS];
};

class ChatServer
{
public:
    ChatServer();
    ~ChatServer();
    void init();
    void start();
    void coreFunc();

private:
    ChatStat* chat;
};