#include <cstdlib>
#include <ctime>
#include <iostream>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const int target = (std::rand() % 100) + 1;

    std::cout << "[업다운 게임] 1~100 사이 숫자를 맞혀보세요!\n";

    int guess = 0;
    int attempts = 0;

    while (true) {
        std::cout << "숫자 입력: ";
        if (!(std::cin >> guess)) {
            std::cerr << "입력 오류: 정수를 입력하세요.\n";
            return 1;
        }

        if (guess < 1 || guess > 100) {
            std::cout << "범위 오류: 1~100 사이 숫자를 입력하세요.\n";
            continue;
        }

        attempts++;

        if (guess < target) {
            std::cout << "UP!\n";
        } else if (guess > target) {
            std::cout << "DOWN!\n";
        } else {
            std::cout << "정답입니다! 시도 횟수: " << attempts << "\n";
            break;
        }
    }

    return 0;
}
