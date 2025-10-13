#include "server.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

Server::Server(int in_port) : m_port(in_port), m_server_fd(-1) {
    std::cout << "init server on port " << m_port << std::endl;
}

Server::~Server() {
    if (m_server_fd != -1) {
        close(m_server_fd);
    }
}

void Server::start() {
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == -1) {
        throw std::runtime_error("Can't create socket");
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    if (bind(m_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Can't bind socket to port");
    }

    if (listen(m_server_fd, 10) < 0) {
        throw std::runtime_error("Can't start listening on port");
    }
}

void Server::run() {
    std::cout << "Server started. Waiting for connections..." << std::endl;
    std::cout << "Open in browser http://localhost:" << m_port << std::endl;

    while (true) {
        int client_socket = accept(m_server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("connections error");
            continue;
        }
        
        handle_connection(client_socket);
        
        close(client_socket);
    }
}

void Server::handle_connection(int client_socket) {
    const char *http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
    
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);
    std::cout << "--- New request ---\n" << buffer << "\n--------------------------\n";

    write(client_socket, http_response, strlen(http_response));
}
