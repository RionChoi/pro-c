#include <iostream>
#include <memory>
#include <string>
#include <vector>

void runSafePath()
{
    const std::vector<int> numbers{3, 5, 8, 13};
    int sum = 0;

    for (std::size_t index = 0; index < numbers.size(); ++index) {
        sum += numbers.at(index);
    }

    auto buffer = std::make_unique<int[]>(4);
    for (int index = 0; index < 4; ++index) {
        buffer[static_cast<std::size_t>(index)] = sum + index;
    }

    std::cout << "safe sum: " << sum << '\n';
}

void runOverflowDemo()
{
    auto buffer = std::make_unique<int[]>(4);
    for (int index = 0; index <= 4; ++index) {
        buffer[static_cast<std::size_t>(index)] = index * 10;
    }
}

void runUseAfterFreeDemo()
{
    int* raw = new int[2]{7, 9};
    delete[] raw;
    raw[0] = 42;
    std::cout << raw[0] << '\n';
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        const std::string mode = argv[1];

        if (mode == "--demo-overflow") {
            runOverflowDemo();
            return 0;
        }

        if (mode == "--demo-uaf") {
            runUseAfterFreeDemo();
            return 0;
        }
    }

    runSafePath();
    return 0;
}
