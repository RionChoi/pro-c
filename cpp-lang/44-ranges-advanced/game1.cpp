// Session 44: C++20 Ranges 심화 - 작업 큐 최적화 미니 시뮬레이션
#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

struct Job {
    int id;
    std::string type;
    int priority;
    int cost;
    bool ready;
};

int main() {
    std::vector<Job> jobs{
        {1, "compile", 4, 8, true},
        {2, "test", 5, 6, true},
        {3, "deploy", 3, 9, false},
        {4, "lint", 2, 2, true},
        {5, "package", 4, 5, true},
        {6, "audit", 5, 7, false},
        {7, "test", 3, 4, true}
    };

    std::ranges::sort(jobs, [](const Job& a, const Job& b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        return a.cost < b.cost;
    });

    auto schedule = jobs
        | std::views::filter(&Job::ready)
        | std::views::filter([](const Job& job) {
            return job.cost <= 8;
        })
        | std::views::take(4);

    int total_cost = 0;
    std::cout << "=== selected job schedule ===\n";
    for (const Job& job : schedule) {
        total_cost += job.cost;
        std::cout << "#" << job.id << " " << job.type
                  << " priority=" << job.priority
                  << " cost=" << job.cost << '\n';
    }

    std::cout << "total cost: " << total_cost << '\n';
}
