#include "mini_exec.hpp"

#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<int> parseNumbers(const std::string& line)
{
    std::vector<int> numbers;
    std::stringstream stream(line);
    std::string token;

    while (std::getline(stream, token, ',')) {
        std::stringstream tokenStream(token);
        int value = 0;
        tokenStream >> value;
        numbers.push_back(value);
    }

    return numbers;
}

std::vector<int> requirePositive(const std::vector<int>& numbers)
{
    for (int value : numbers) {
        if (value <= 0) {
            throw std::runtime_error("all numbers must be positive");
        }
    }

    return numbers;
}

int sumNumbers(const std::vector<int>& numbers)
{
    return std::accumulate(numbers.begin(), numbers.end(), 0);
}

int main()
{
    auto pipeline = mini_exec::just(std::string("10, 20, 30, 40"))
                  | mini_exec::then(parseNumbers)
                  | mini_exec::then(requirePositive)
                  | mini_exec::then(sumNumbers);

    std::cout << "native senders available: "
              << (mini_exec::nativeSendersAvailable ? "yes" : "no") << '\n';
    std::cout << "sum: " << mini_exec::sync_wait(std::move(pipeline)) << '\n';
    return 0;
}
