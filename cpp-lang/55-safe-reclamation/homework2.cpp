#include "reclaim.hpp"

#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct Config {
    int version;
    std::string mode;
    int limit;
};

class ConfigStore {
public:
    explicit ConfigStore(Config initial)
        : current_(new Config(std::move(initial)))
    {
    }

    ~ConfigStore()
    {
        delete current_.load(std::memory_order_acquire);
        epochs_.drainUnsafe();
    }

    std::string snapshot() const
    {
        reclaim::EpochDomain::Guard guard(epochs_);
        const Config* config = current_.load(std::memory_order_acquire);

        std::ostringstream out;
        out << "v" << config->version << ' ' << config->mode << " limit=" << config->limit;
        return out.str();
    }

    void publish(Config next)
    {
        Config* fresh = new Config(std::move(next));
        Config* old = current_.exchange(fresh, std::memory_order_acq_rel);
        epochs_.retire(old);
    }

    std::size_t synchronize()
    {
        return epochs_.synchronize();
    }

    std::size_t pending() const
    {
        return epochs_.pending();
    }

private:
    std::atomic<Config*> current_;
    mutable reclaim::EpochDomain epochs_;
};

int main()
{
    ConfigStore store(Config{1, "safe", 64});

    auto reader = std::async(std::launch::async, [&store] {
        std::vector<std::string> seen;
        for (int round = 0; round < 4; ++round) {
            seen.push_back(store.snapshot());
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        return seen;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    store.publish(Config{2, "warm", 96});
    const std::size_t reclaimed1 = store.synchronize();

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    store.publish(Config{3, "burst", 128});
    const std::size_t reclaimed2 = store.synchronize();

    std::vector<std::string> seen = reader.get();
    const std::size_t reclaimed3 = store.synchronize();
    const std::size_t reclaimed4 = store.synchronize();

    assert(!seen.empty());
    assert(store.pending() == 0U);

    std::cout << "snapshots:\n";
    for (const std::string& line : seen) {
        std::cout << "  " << line << '\n';
    }
    std::cout << "reclaimed per sync: "
              << reclaimed1 << ", " << reclaimed2 << ", " << reclaimed3 << ", " << reclaimed4
              << "\npending retired: " << store.pending() << '\n';
    return 0;
}
