#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <memory>

template <typename T>
class PoolAllocator {
private:
    std::vector<std::byte> pool;
    std::vector<bool> used;
    static constexpr size_t ELEMENT_SIZE = sizeof(T);
    static constexpr size_t POOL_SIZE = 1024;

public:
    PoolAllocator() : pool(ELEMENT_SIZE * POOL_SIZE), used(POOL_SIZE, false) {}

    T* allocate() {
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            if (!used[i]) {
                used[i] = true;
                return reinterpret_cast<T*>(pool.data() + i * ELEMENT_SIZE);
            }
        }
        throw std::bad_alloc();
    }

    void deallocate(T* ptr) {
        auto offset = reinterpret_cast<std::byte*>(ptr) - pool.data();
        size_t index = static_cast<size_t>(offset) / ELEMENT_SIZE;
        if (index < POOL_SIZE) {
            used[index] = false;
        }
    }
};

struct TypeStats {
    std::string type_name;
    int count = 0;
    double sum = 0.0;
    double min_val = 1e9;
    double max_val = -1e9;
};

class TypeSafeStorage {
private:
    using StorageType = std::variant<int, double, std::string>;
    std::vector<StorageType> items;
    std::vector<TypeStats> stats;

    TypeStats* getOrCreateStats(const std::string& type_name) {
        auto it = std::find_if(stats.begin(), stats.end(),
            [&](const TypeStats& s) { return s.type_name == type_name; });
        if (it != stats.end()) {
            return &(*it);
        }
        stats.push_back(TypeStats{type_name, 0, 0.0, 1e9, -1e9});
        return &stats.back();
    }

    void updateStats(const StorageType& item) {
        if (std::holds_alternative<int>(item)) {
            int val = std::get<int>(item);
            TypeStats* stat = getOrCreateStats("int");
            stat->count++;
            stat->sum += static_cast<double>(val);
            stat->min_val = std::min(stat->min_val, static_cast<double>(val));
            stat->max_val = std::max(stat->max_val, static_cast<double>(val));
        } else if (std::holds_alternative<double>(item)) {
            double val = std::get<double>(item);
            TypeStats* stat = getOrCreateStats("double");
            stat->count++;
            stat->sum += val;
            stat->min_val = std::min(stat->min_val, val);
            stat->max_val = std::max(stat->max_val, val);
        } else if (std::holds_alternative<std::string>(item)) {
            const std::string& val = std::get<std::string>(item);
            TypeStats* stat = getOrCreateStats("string");
            stat->count++;
            double len = static_cast<double>(val.length());
            stat->sum += len;
            stat->min_val = std::min(stat->min_val, len);
            stat->max_val = std::max(stat->max_val, len);
        }
    }

public:
    void insert(int value) {
        items.push_back(value);
        updateStats(items.back());
    }

    void insert(double value) {
        items.push_back(value);
        updateStats(items.back());
    }

    void insert(const std::string& value) {
        items.push_back(value);
        updateStats(items.back());
    }

    void insert(const char* value) {
        insert(std::string(value));
    }

    std::vector<std::byte> serialize() const {
        std::vector<std::byte> buffer;

        uint32_t item_count = static_cast<uint32_t>(items.size());
        buffer.insert(buffer.end(),
            reinterpret_cast<std::byte*>(&item_count),
            reinterpret_cast<std::byte*>(&item_count) + sizeof(item_count));

        for (const auto& item : items) {
            if (std::holds_alternative<int>(item)) {
                std::byte type_byte = std::byte(1);
                buffer.push_back(type_byte);
                int val = std::get<int>(item);
                buffer.insert(buffer.end(),
                    reinterpret_cast<std::byte*>(&val),
                    reinterpret_cast<std::byte*>(&val) + sizeof(val));
            } else if (std::holds_alternative<double>(item)) {
                std::byte type_byte = std::byte(2);
                buffer.push_back(type_byte);
                double val = std::get<double>(item);
                buffer.insert(buffer.end(),
                    reinterpret_cast<std::byte*>(&val),
                    reinterpret_cast<std::byte*>(&val) + sizeof(val));
            } else if (std::holds_alternative<std::string>(item)) {
                std::byte type_byte = std::byte(3);
                buffer.push_back(type_byte);
                const std::string& val = std::get<std::string>(item);
                uint32_t str_len = static_cast<uint32_t>(val.length());
                buffer.insert(buffer.end(),
                    reinterpret_cast<std::byte*>(&str_len),
                    reinterpret_cast<std::byte*>(&str_len) + sizeof(str_len));
                buffer.insert(buffer.end(),
                    reinterpret_cast<const std::byte*>(val.data()),
                    reinterpret_cast<const std::byte*>(val.data()) + val.length());
            }
        }
        return buffer;
    }

    void deserialize(const std::vector<std::byte>& buffer) {
        items.clear();
        stats.clear();

        if (buffer.size() < sizeof(uint32_t)) return;

        uint32_t item_count;
        std::memcpy(&item_count, buffer.data(), sizeof(item_count));

        size_t pos = sizeof(item_count);
        for (uint32_t i = 0; i < item_count && pos < buffer.size(); ++i) {
            std::byte type_byte = buffer[pos++];

            if (type_byte == std::byte(1)) {
                if (pos + sizeof(int) <= buffer.size()) {
                    int val;
                    std::memcpy(&val, buffer.data() + pos, sizeof(val));
                    pos += sizeof(val);
                    insert(val);
                }
            } else if (type_byte == std::byte(2)) {
                if (pos + sizeof(double) <= buffer.size()) {
                    double val;
                    std::memcpy(&val, buffer.data() + pos, sizeof(val));
                    pos += sizeof(val);
                    insert(val);
                }
            } else if (type_byte == std::byte(3)) {
                if (pos + sizeof(uint32_t) <= buffer.size()) {
                    uint32_t str_len;
                    std::memcpy(&str_len, buffer.data() + pos, sizeof(str_len));
                    pos += sizeof(str_len);
                    if (pos + str_len <= buffer.size()) {
                        std::string val(reinterpret_cast<const char*>(buffer.data() + pos), str_len);
                        pos += str_len;
                        insert(val);
                    }
                }
            }
        }
    }

    void print() const {
        std::cout << "=== Storage Statistics ===" << std::endl;
        for (const auto& stat : stats) {
            std::cout << "Type: " << stat.type_name << std::endl;
            std::cout << "  Count: " << stat.count << std::endl;
            if (stat.count > 0) {
                std::cout << "  Average: " << (stat.sum / stat.count) << std::endl;
                std::cout << "  Min: " << stat.min_val << std::endl;
                std::cout << "  Max: " << stat.max_val << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << "Total items: " << items.size() << std::endl;
    }
};

int main() {
    std::cout << "=== Type-Safe Container Library ===" << std::endl << std::endl;

    TypeSafeStorage store;

    std::cout << "Inserting mixed types..." << std::endl;
    store.insert(42);
    store.insert(100);
    store.insert(75);
    store.insert(3.14);
    store.insert(2.71);
    store.insert(1.41);
    store.insert("hello");
    store.insert("world");
    store.insert("cpp");
    std::cout << std::endl;

    std::cout << "Original storage statistics:" << std::endl;
    store.print();

    std::cout << "Serializing to binary..." << std::endl;
    auto buffer = store.serialize();
    std::cout << "Serialized size: " << buffer.size() << " bytes" << std::endl << std::endl;

    std::cout << "Deserializing from binary..." << std::endl;
    TypeSafeStorage restored;
    restored.deserialize(buffer);
    std::cout << std::endl;

    std::cout << "Restored storage statistics:" << std::endl;
    restored.print();

    return 0;
}
