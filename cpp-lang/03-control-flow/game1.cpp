#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

int main(void) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int target = std::rand() % 100 + 1;
    int guess;
    const int maxAttempts = 7;

    std::cout << "=== 숫자 맞추기 게임 ===\n";
    std::cout << "1~100 사이의 숫자를 맞추세요!\n";
    std::cout << "기회: " << maxAttempts << "회\n\n";

    for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
        int remaining = maxAttempts - attempt + 1;
        std::cout << "시도 " << attempt << "/" << maxAttempts
                  << " (남은 기회: " << remaining << ")\n";
        std::cout << "숫자를 입력하세요: ";

        if (!(std::cin >> guess)) {
            std::cerr << "입력 오류: 숫자를 입력하세요.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            --attempt;
            continue;
        }

        if (guess < 1 || guess > 100) {
            std::cout << "1~100 사이의 숫자만 입력하세요.\n\n";
            --attempt;
            continue;
        }

        if (guess == target) {
            std::cout << "\n축하합니다! " << attempt << "번 만에 맞추셨습니다!\n";
            return 0;
        } else if (guess < target) {
            std::cout << "더 큰 수입니다.\n\n";
        } else {
            std::cout << "더 작은 수입니다.\n\n";
        }
    }

    std::cout << "\n기회를 모두 사용했습니다!\n";
    std::cout << "정답은 " << target << "였습니다.\n";
    return 0;
}
