#include <algorithm>
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

int main() {
    std::vector<int> numbers;
    for (int n = 1; n <= 9; ++n) {
        numbers.push_back(n);
    }

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::shuffle(numbers.begin(), numbers.end(), rng);

    const int target = numbers[0];
    std::cout << "=== 숫자 빙고 게임 ===\n";
    std::cout << "1~9 중 숨겨진 숫자를 맞혀보세요. 기회는 3번입니다.\n";

    bool win = false;
    for (int attempt = 1; attempt <= 3; ++attempt) {
        std::cout << "(" << attempt << "/3) 입력: ";
        int guess = 0;
        if (!(std::cin >> guess)) {
            std::cerr << "입력 오류: 정수를 입력하세요.\n";
            return 1;
        }

        if (guess == target) {
            win = true;
            std::cout << "정답입니다!\n";
            break;
        }

        std::cout << "틀렸습니다. "
                  << ((guess < target) ? "더 큰 수입니다.\n" : "더 작은 수입니다.\n");
    }

    if (!win) {
        std::cout << "아쉽네요! 정답은 " << target << "였습니다.\n";
    }

    return 0;
}
