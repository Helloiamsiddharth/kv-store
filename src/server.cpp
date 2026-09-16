#include "server.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

TCPServer::TCPServer(int port, KeyValueStore& store)
    : port_(port), server_fd_(-1), store_(store), running_(false) {}

TCPServer::~TCPServer() {
    stop();
}

void TCPServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        perror("Socket creation failed");
        return;
    }

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return;
    }

    if (listen(server_fd_, 10) < 0) {
        perror("Listen failed");
        return;
    }

    running_ = true;
    std::cout << "[Server] Listening on port " << port_ << "..." << std::endl;

    while (running_) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &addr_len);
        
        if (client_fd < 0) {
            if (!running_) break;
            continue;
        }

        worker_threads_.emplace_back(&TCPServer::handle_client, this, client_fd);
    }
}

void TCPServer::stop() {
    running_ = false;
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
    for (auto& t : worker_threads_) {
        if (t.joinable()) t.join();
    }
}

void TCPServer::handle_client(int client_fd) {
    char buffer[1024];
    while (running_) {
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;

        buffer[bytes_read] = '\0';
        std::string response = process_command(std::string(buffer));
        write(client_fd, response.c_str(), response.size());
    }
    close(client_fd);
}

std::string TCPServer::process_command(const std::string& raw_cmd) {
    std::stringstream ss(raw_cmd);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) tokens.push_back(token);

    if (tokens.empty()) return "-ERR empty command\r\n";

    std::string cmd = tokens[0];
    if (cmd == "SET" || cmd == "set") {
        if (tokens.size() < 3) return "-ERR SET requires key and value\r\n";
        std::optional<int> ttl = std::nullopt;
        if (tokens.size() >= 4) ttl = std::stoi(tokens[3]);
        store_.set(tokens[1], tokens[2], ttl);
        return "+OK\r\n";
    } else if (cmd == "GET" || cmd == "get") {
        if (tokens.size() < 2) return "-ERR GET requires a key\r\n";
        auto val = store_.get(tokens[1]);
        if (!val) return "$-1\r\n";
        return "$" + std::to_string(val->length()) + "\r\n" + *val + "\r\n";
    } else if (cmd == "DEL" || cmd == "del") {
        if (tokens.size() < 2) return "-ERR DEL requires a key\r\n";
        bool removed = store_.del(tokens[1]);
        return ":" + std::to_string(removed ? 1 : 0) + "\r\n";
    }

    return "-ERR unknown command\r\n";
}