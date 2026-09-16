#include "store.hpp"
#include <cassert>
#include <iostream>
#include <thread>

void test_set_get() {
    KeyValueStore kv;
    kv.set("name", "Alice");
    assert(kv.get("name").value_or("") == "Alice");
    std::cout << "[PASS] Test Set/Get" << std::endl;
}

void test_ttl_expiry() {
    KeyValueStore kv;
    kv.set("session", "xyz123", 100); // 100ms TTL
    assert(kv.get("session").has_value());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    assert(!kv.get("session").has_value());
    std::cout << "[PASS] Test TTL Expiry" << std::endl;
}

int main() {
    test_set_get();
    test_ttl_expiry();
    std::cout << "All unit tests passed successfully!" << std::endl;
    return 0;
}