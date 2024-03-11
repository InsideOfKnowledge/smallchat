#include <string>

#define MAX_CLIENTS 1000
#define SERVER_PORT 7711

class Client
{
public:
    Client(int fd_, std::string unic_);

private:
    int fd;
    std::string unic;
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