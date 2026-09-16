#include "store.hpp"
#include "server.hpp"
#include <iostream>

int main() {
    KeyValueStore store;
    TCPServer server(6379, store);
    
    std::cout << "Starting Key-Value Store Server..." << std::endl;
    server.start();

    return 0;
}