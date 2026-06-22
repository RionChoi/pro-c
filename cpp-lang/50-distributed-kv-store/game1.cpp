// Session 50: Distributed KV Store - cluster simulation game
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

class ClusterGame {
public:
    ClusterGame()
        : nodes_{{"alpha", true, {}}, {"beta", true, {}}, {"gamma", true, {}}} {}

    void put(const std::string& key, const std::string& value) {
        for (Node* node : replicas(key)) {
            if (node->online) {
                node->data[key] = value;
            }
        }
        ++writes_;
    }

    std::optional<std::string> get(const std::string& key) const {
        for (const Node* node : replicas(key)) {
            if (!node->online) {
                continue;
            }
            if (auto it = node->data.find(key); it != node->data.end()) {
                ++reads_;
                return it->second;
            }
        }
        ++misses_;
        return std::nullopt;
    }

    void erase(const std::string& key) {
        for (Node* node : replicas(key)) {
            node->data.erase(key);
        }
    }

    void set_online(const std::string& id, bool online) {
        auto it = std::ranges::find_if(nodes_, [&](const Node& node) {
            return node.id == id;
        });
        if (it != nodes_.end()) {
            it->online = online;
        }
    }

    void print_status() const {
        std::cout << "reads=" << reads_ << " writes=" << writes_
                  << " misses=" << misses_ << '\n';
        for (const Node& node : nodes_) {
            std::cout << "  " << node.id << " online=" << node.online
                      << " keys=" << node.data.size() << '\n';
        }
    }

private:
    std::vector<Node*> replicas(const std::string& key) {
        const std::size_t start = std::hash<std::string>{}(key) % nodes_.size();
        return {&nodes_[start], &nodes_[(start + 1) % nodes_.size()]};
    }

    std::vector<const Node*> replicas(const std::string& key) const {
        const std::size_t start = std::hash<std::string>{}(key) % nodes_.size();
        return {&nodes_[start], &nodes_[(start + 1) % nodes_.size()]};
    }

    std::vector<Node> nodes_;
    mutable int reads_{0};
    int writes_{0};
    mutable int misses_{0};
};

int main() {
    ClusterGame game;

    std::cout << "=== distributed KV simulation ===\n";
    game.put("player:1", "level=7");
    game.put("player:2", "level=3");
    game.put("config:rate", "0.25");
    game.print_status();

    game.set_online("beta", false);
    std::cout << "\nbeta offline\n";
    for (const auto& key : {"player:1", "player:2", "missing"}) {
        auto value = game.get(key);
        std::cout << "GET " << key << " -> "
                  << (value ? *value : "<miss>") << '\n';
    }

    game.erase("player:2");
    game.set_online("beta", true);
    std::cout << "\nfinal status\n";
    game.print_status();
}
