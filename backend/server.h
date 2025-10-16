#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <map>
#include <string>

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
    std::string get_request_method(const char* in_buffer);
    void handle_connection(int in_client_socket);
    int m_port;
    int m_server_fd;
    // FIXME change to DB storage
    std::map<std::string, std::string> m_labels;
};

#endif  // SERVER_H
