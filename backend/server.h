#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

class Server {
public:
    explicit Server(int in_port);
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    ~Server();
    void start();
    void run();
private:
    std::string get_request_path(const char* in_buffer);
    void handle_connection(int in_client_socket);
    int m_port;
    int m_server_fd;
};

#endif // SERVER_HPP
