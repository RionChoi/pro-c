#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct ScoreBoard {
    void addSafe(int score)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        total_ += score;
    }

    void addUnsafe(int score)
    {
        total_ += score;
    }

    int total() const
    {
        return total_;
    }

private:
    int total_ = 0;
    std::mutex mutex_;
};

void runSafeMatch()
{
    ScoreBoard board;
    std::vector<std::thread> workers;

    for (int player = 0; player < 4; ++player) {
        workers.emplace_back([&board, player] {
            for (int round = 0; round < 1000; ++round) {
                board.addSafe(player + 1);
            }
        });
    }

    for (std::thread& worker : workers) {
        worker.join();
    }

    std::cout << "safe total: " << board.total() << '\n';
}

void runRaceDemo()
{
    ScoreBoard board;
    std::thread left([&board] {
        for (int round = 0; round < 5000; ++round) {
            board.addUnsafe(1);
        }
    });
    std::thread right([&board] {
        for (int round = 0; round < 5000; ++round) {
            board.addUnsafe(1);
        }
    });

    left.join();
    right.join();
    std::cout << "racy total: " << board.total() << '\n';
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        const std::string mode = argv[1];
        if (mode == "--demo-race") {
            runRaceDemo();
            return 0;
        }
    }

    runSafeMatch();
    return 0;
}
