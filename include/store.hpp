#ifndef STORE_HPP
#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <chrono>

class KeyValueStore {
public:
    struct ValueEntry {
        std::string value;
        std::optional<std::chrono::steady_clock::time_point> expiry;
    };

    void set(const std::string& key, const std::string& value, std::optional<int> ttl_ms = std::nullopt);
    std::optional<std::string> get(const std::string& key);
    bool del(const std::string& key);
    void cleanup_expired();

private:
    std::unordered_map<std::string, ValueEntry> store_;
    mutable std::shared_mutex mutex_;
};

#endif // STORE_HPP