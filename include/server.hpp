#ifndef SERVER_HPP
#define SERVER_HPP

#include "store.hpp"
#include <atomic>
#include <thread>
#include <vector>

class TCPServer {
public:
    TCPServer(int port, KeyValueStore& store);
    ~TCPServer();

    void start();
    void stop();

private:
    void handle_client(int client_fd);
    std::string process_command(const std::string& raw_cmd);

    int port_;
    int server_fd_;
    KeyValueStore& store_;
    std::atomic<bool> running_;
    std::vector<std::thread> worker_threads_;
};

#endif // SERVER_HPP