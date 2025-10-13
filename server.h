#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
public:
    explicit Server(int in_port);
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    ~Server();
    void start();
    void run();
private:
    void handle_connection(int in_client_socket);

    int m_port;
    int m_server_fd;
};

#endif // SERVER_HPP
