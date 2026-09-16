# High-Performance In-Memory Key-Value Store (C++17)

A lightweight, concurrent in-memory key-value database built in C++17 supporting key expiration (TTL) and custom TCP client connections.

## Features
- **Concurrent Access:** Thread-safe operations using `std::shared_mutex` for lock-free read concurrency.
- **TTL Expiration:** Supports millisecond-precision key expiration.
- **TCP Socket Server:** Simple socket-based command listener inspired by the Redis protocol.

## Building and Running

### Prerequisites
- CMake 3.16+
- C++17 compliant compiler (`g++` or `clang++`)

### Build Steps
```bash
mkdir build && cd build
cmake ..
make