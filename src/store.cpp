#include "store.hpp"

void KeyValueStore::set(const std::string& key, const std::string& value, std::optional<int> ttl_ms) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    std::optional<std::chrono::steady_clock::time_point> expiry_time;
    if (ttl_ms.has_value()) {
        expiry_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(ttl_ms.value());
    }
    
    store_[key] = ValueEntry{value, expiry_time};
}

std::optional<std::string> KeyValueStore::get(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    auto it = store_.find(key);
    if (it == store_.end()) {
        return std::nullopt;
    }

    if (it->second.expiry.has_value() && std::chrono::steady_clock::now() > it->second.expiry.value()) {
        return std::nullopt;
    }

    return it->second.value;
}

bool KeyValueStore::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    return store_.erase(key) > 0;
}

void KeyValueStore::cleanup_expired() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    for (auto it = store_.begin(); it != store_.end(); ) {
        if (it->second.expiry.has_value() && now > it->second.expiry.value()) {
            it = store_.erase(it);
        } else {
            ++it;
        }
    }
}