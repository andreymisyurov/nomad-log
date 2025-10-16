#include "server.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

#include "cpp-logger/Logger.h"

using json = nlohmann::json;

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

    if (bind(m_server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
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
    std::string method = get_request_method(buffer);

    if (path == "/api/labels") {
        // FIXME change to proxy logic
        if (method == "OPTIONS") {
            std::string resp =
                "HTTP/1.1 204 No Content\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Access-Control-Max-Age: 86400\r\n\r\n";
            write(client_socket, resp.c_str(), resp.length());
            return;
        } else if (method == "POST") {
            std::string body = std::string(buffer);
            size_t pos = body.find("\r\n\r\n");
            if (pos != std::string::npos) {
                body = body.substr(pos + 4);
                try {
                    json j = json::parse(body);
                    m_labels.clear();
                    for (auto& [key, val] : j.items()) {
                        m_labels[key] = val.get<std::string>();
                    }
                    LOG_INFO("Saved {} labels", m_labels.size());
                } catch (const json::exception& e) {
                    LOG_ERROR("JSON parse error: {}", e.what());
                }
            }

            std::string resp =
                "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: 2\r\n\r\nOK";
            write(client_socket, resp.c_str(), resp.length());
            return;
        } else if (method == "GET") {
            json j = m_labels;
            std::string json_str = j.dump();

            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\nContent-Type: "
                        "application/json\r\nAccess-Control-Allow-Origin: *\r\n";
            response << "Content-Length: " << json_str.length() << "\r\n\r\n";
            response << json_str;

            write(client_socket, response.str().c_str(), response.str().length());
            return;
        }
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

    return path;
}

std::string Server::get_request_method(const char* in_buffer) {
    std::string request_line;
    std::istringstream stream(in_buffer);
    std::getline(stream, request_line);

    std::istringstream line_stream(request_line);
    std::string method;
    line_stream >> method;
    return method;
}