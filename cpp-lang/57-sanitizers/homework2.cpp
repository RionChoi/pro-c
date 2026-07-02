#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

int checkedAdd(int left, int right)
{
    if (right > 0 && left > std::numeric_limits<int>::max() - right) {
        throw std::overflow_error("signed overflow blocked");
    }
    if (right < 0 && left < std::numeric_limits<int>::min() - right) {
        throw std::overflow_error("signed underflow blocked");
    }

    return left + right;
}

unsigned int checkedShift(unsigned int value, unsigned int amount)
{
    if (amount >= 32U) {
        throw std::out_of_range("shift count too large");
    }

    return value << amount;
}

int overflowDemo()
{
    volatile int left = std::numeric_limits<int>::max();
    volatile int right = 1;
    return left + right;
}

int shiftDemo()
{
    volatile int amount = -1;
    return 1 << amount;
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        const std::string mode = argv[1];

        if (mode == "--demo-overflow") {
            std::cout << overflowDemo() << '\n';
            return 0;
        }

        if (mode == "--demo-shift") {
            std::cout << shiftDemo() << '\n';
            return 0;
        }
    }

    std::cout << "checked add: " << checkedAdd(120, 30) << '\n';
    std::cout << "checked shift: " << checkedShift(3U, 4U) << '\n';
    return 0;
}
