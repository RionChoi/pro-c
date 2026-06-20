// Session 50: Distributed KV Store - sharding, replication, quorum read
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Node {
    std::string id;
    bool online{true};
    std::unordered_map<std::string, std::string> data;
};

class Cluster {
public:
    explicit Cluster(std::vector<std::string> node_ids, std::size_t replication_factor)
        : replication_factor_(replication_factor) {
        for (auto& id : node_ids) {
            nodes_.push_back(Node{std::move(id), true, {}});
        }
    }

    void set_online(const std::string& id, bool online) {
        if (Node* node = find_node(id)) {
            node->online = online;
        }
    }

    void put(const std::string& key, const std::string& value) {
        for (Node* node : replicas_for(key)) {
            node->data[key] = value;
        }
    }

    bool erase(const std::string& key) {
        bool erased = false;
        for (Node* node : replicas_for(key)) {
            erased = node->data.erase(key) > 0 || erased;
        }
        return erased;
    }

    std::optional<std::string> get(const std::string& key) const {
        for (const Node* node : replicas_for(key)) {
            if (!node->online) {
                continue;
            }
            if (auto it = node->data.find(key); it != node->data.end()) {
                return it->second;
            }
        }
        return std::nullopt;
    }

    void print_layout() const {
        for (const Node& node : nodes_) {
            std::cout << node.id << " online=" << node.online << " keys:";
            std::vector<std::string> keys;
            for (const auto& [key, _] : node.data) {
                keys.push_back(key);
            }
            std::ranges::sort(keys);
            for (const auto& key : keys) {
                std::cout << ' ' << key;
            }
            std::cout << '\n';
        }
    }

private:
    Node* find_node(const std::string& id) {
        auto it = std::ranges::find_if(nodes_, [&](const Node& node) {
            return node.id == id;
        });
        return it == nodes_.end() ? nullptr : &*it;
    }

    std::vector<Node*> replicas_for(const std::string& key) {
        std::vector<Node*> out;
        if (nodes_.empty()) {
            return out;
        }

        const std::size_t start = std::hash<std::string>{}(key) % nodes_.size();
        const std::size_t count = std::min(replication_factor_, nodes_.size());
        for (std::size_t i = 0; i < count; ++i) {
            out.push_back(&nodes_[(start + i) % nodes_.size()]);
        }
        return out;
    }

    std::vector<const Node*> replicas_for(const std::string& key) const {
        std::vector<const Node*> out;
        if (nodes_.empty()) {
            return out;
        }

        const std::size_t start = std::hash<std::string>{}(key) % nodes_.size();
        const std::size_t count = std::min(replication_factor_, nodes_.size());
        for (std::size_t i = 0; i < count; ++i) {
            out.push_back(&nodes_[(start + i) % nodes_.size()]);
        }
        return out;
    }

    std::vector<Node> nodes_;
    std::size_t replication_factor_;
};

int main() {
    Cluster cluster{{"node-a", "node-b", "node-c"}, 2};

    cluster.put("user:1", "Rion");
    cluster.put("cart:1", "book,keyboard");
    cluster.put("flag:beta", "enabled");

    std::cout << "=== initial layout ===\n";
    cluster.print_layout();

    cluster.set_online("node-b", false);
    std::cout << "\nnode-b is offline\n";
    for (const auto& key : {"user:1", "cart:1", "flag:beta"}) {
        auto value = cluster.get(key);
        std::cout << "GET " << key << " -> "
                  << (value ? *value : "<miss>") << '\n';
    }

    cluster.erase("cart:1");
    std::cout << "\n=== after delete cart:1 ===\n";
    cluster.print_layout();
}
