#include <string>
#include <vector>

#define MAX_CLIENTS 1000
#define SERVER_PORT 7711

class Client final {
    public:
        Client() = default;
        ~Client();;
        Client(const Client& cl) = delete;
        Client(Client&& cl) = delete;

        void setFd(int fd);
        int getFd() const;
        void setNick(const std::string& nick);
        const std::string& getNick() const;

    private:
        int             _fd;
        std::string     _nick;
};


class ChatServer final {
    public:
        ~ChatServer();

        static ChatServer& getInstance();
        bool start();    
        
    private:
        ChatServer();
        ChatServer(const ChatServer& cs) = delete;
        ChatServer(ChatServer&& cs) = delete;
        ChatServer& operator = (const ChatServer& cs) = delete;

        bool init();
        int acceptClient();
        void sendWelcomeToClient();
        int socketSetNonBlockNoDelay(int fd);

    private:
        int                     _serverSock;
        int                     _numClients;
        int                     _maxClient;
        std::vector<Client*>    _clientVec;
};