#include <iostream>
#include <limits>
#include <string>

template <typename T>
int compareValues(const T& lhs, const T& rhs) {
    if (lhs < rhs) {
        return -1;
    }
    if (lhs > rhs) {
        return 1;
    }
    return 0;
}

int main() {
    std::cout << "=== 템플릿 비교 게임 ===\n";
    std::cout << "숫자 2개를 입력해 비교 결과를 맞혀보세요.\n";

    int score = 0;
    const int rounds = 3;

    for (int round = 1; round <= rounds; ++round) {
        int x = 0;
        int y = 0;
        std::cout << "\n[라운드 " << round << "] 숫자 2개 입력: ";
        if (!(std::cin >> x >> y)) {
            std::cerr << "입력 오류가 발생했습니다.\n";
            return 1;
        }

        std::cout << "예상 결과 입력 (<, >, =): ";
        char guess = '\0';
        if (!(std::cin >> guess)) {
            std::cerr << "입력 오류가 발생했습니다.\n";
            return 1;
        }

        const int result = compareValues(x, y);
        bool correct = false;
        if (guess == '<' && result < 0) {
            correct = true;
        } else if (guess == '>' && result > 0) {
            correct = true;
        } else if (guess == '=' && result == 0) {
            correct = true;
        }

        if (correct) {
            ++score;
            std::cout << "정답!\n";
        } else {
            std::cout << "오답! 정답은 "
                      << ((result < 0) ? "<" : (result > 0 ? ">" : "="))
                      << " 입니다.\n";
        }
    }

    std::cout << "\n최종 점수: " << score << " / " << rounds << "\n";
    return 0;
}
