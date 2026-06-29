#include "reclaim.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct Quest {
    std::string name;
    int score;
};

struct TeamScore {
    std::string team;
    int total = 0;
};

int main()
{
    reclaim::AtomicStack<Quest> board;
    board.push(Quest{"dragon", 90});
    board.push(Quest{"golem", 55});
    board.push(Quest{"witch", 40});
    board.push(Quest{"slime", 10});
    board.push(Quest{"lich", 75});
    board.push(Quest{"wolf", 20});

    std::mutex scoreMutex;
    std::vector<TeamScore> scores{{"red", 0}, {"blue", 0}};

    auto worker = [&board, &scores, &scoreMutex](std::size_t index) {
        while (true) {
            std::optional<Quest> quest = board.pop();
            if (!quest.has_value()) {
                return;
            }

            std::lock_guard<std::mutex> lock(scoreMutex);
            scores[index].total += quest->score;
        }
    };

    std::thread red(worker, 0U);
    std::thread blue(worker, 1U);
    red.join();
    blue.join();

    const std::size_t reclaimed = board.reclaimNow();

    std::ranges::sort(scores, [](const TeamScore& left, const TeamScore& right) {
        return left.total > right.total;
    });

    const int total = scores[0].total + scores[1].total;
    assert(total == 290);

    for (const TeamScore& score : scores) {
        std::cout << score.team << " total=" << score.total << '\n';
    }
    std::cout << "reclaimed now: " << reclaimed
              << "\npending retired: " << board.pendingRetired() << '\n';
    return 0;
}
