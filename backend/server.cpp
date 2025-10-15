#include "server.h"
#include "cpp-logger/Logger.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <sstream>

Server::Server(int in_port) : m_port(in_port), m_server_fd(-1) {
    LOG_INFO("init server on port {}", m_port);
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
    LOG_INFO("Server started. Open in browser: http://localhost:{}", m_port);

    while (true) {
        int client_socket = accept(m_server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            LOG_ERROR("connections error");
            continue;
        }
        LOG_INFO("new connection from {}", client_socket);
        handle_connection(client_socket);
        
        close(client_socket);
    }
}

// refactor this
void Server::handle_connection(int client_socket) {
    char buffer[4096] = {0};
    read(client_socket, buffer, 4096);
    LOG_INFO("New request:\n---\n{}\n---", buffer);

    std::string path = get_request_path(buffer);
    std::string file_path = "web" + path;
    
    std::ifstream file(file_path);
    if (file.good()) {
        std::stringstream file_buffer;
        file_buffer << file.rdbuf();
        std::string file_content = file_buffer.str();

        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << file_content.length() << "\r\n";
        response << "\r\n";
        response << file_content;

        write(client_socket, response.str().c_str(), response.str().length());
        LOG_INFO("Sent file: {}", file_path);

    } else {
        std::string response_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        write(client_socket, response_404.c_str(), response_404.length());
        LOG_WARN("File not found: {}", file_path);
    }
}

// refactor this
std::string Server::get_request_path(const char* in_buffer) {
    std::string request_line;
    std::istringstream stream(in_buffer);
    std::getline(stream, request_line);

    std::istringstream line_stream(request_line);
    std::string method, path;
    line_stream >> method >> path;

    if (path == "/") {
        return "/index.html";
    }
    return path;
}