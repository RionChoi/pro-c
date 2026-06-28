#include "mini_exec.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct Job {
    std::string name;
    int priority;
    int estimateMs;
};

struct Report {
    std::string name;
    int priority;
    std::string summary;
};

Job normalize(Job job)
{
    if (job.name.empty()) {
        throw std::runtime_error("job name is required");
    }
    if (job.priority < 1 || job.priority > 5) {
        throw std::runtime_error("priority must be between 1 and 5");
    }
    if (job.estimateMs <= 0) {
        throw std::runtime_error("estimate must be positive");
    }
    return job;
}

Report execute(const Job& job)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(job.estimateMs));

    std::ostringstream out;
    out << "done by " << std::this_thread::get_id() << " in " << job.estimateMs << "ms";
    return Report{job.name, job.priority, out.str()};
}

int main()
{
    mini_exec::ThreadPool pool(3);
    const std::vector<Job> jobs{
        {"lint", 2, 30},
        {"benchmark", 5, 70},
        {"package", 3, 50},
    };

    std::vector<std::future<Report>> futures;
    futures.reserve(jobs.size());

    for (const Job& job : jobs) {
        auto sender = mini_exec::just(job)
                    | mini_exec::then(normalize)
                    | mini_exec::then(execute);
        futures.push_back(mini_exec::start(pool, std::move(sender)));
    }

    std::vector<Report> reports;
    reports.reserve(futures.size());
    for (std::future<Report>& future : futures) {
        reports.push_back(future.get());
    }

    std::ranges::sort(reports, [](const Report& left, const Report& right) {
        return left.priority > right.priority;
    });

    for (const Report& report : reports) {
        std::cout << '[' << report.priority << "] " << report.name
                  << " -> " << report.summary << '\n';
    }

    return 0;
}
