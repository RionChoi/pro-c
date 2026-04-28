#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>

bool contains_duplicate(const std::string& s) {
    std::vector<char> chars;
    for (char c : s) {
        if (std::find(chars.begin(), chars.end(), c) != chars.end()) {
            return true;
        }
        chars.push_back(c);
    }
    return false;
}

std::string generate_answer(void) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::string ans;
    while (ans.size() < 3) {
        char digit = static_cast<char>('0' + (std::rand() % 10));
        ans += digit;
        if (contains_duplicate(ans)) {
            ans.pop_back();
        }
    }
    return ans;
}

void get_guess(std::string& guess) {
    std::cout << "3자리 숫자를 입력하세요 (중복 없이): ";
    std::cin >> guess;
    while (guess.size() != 3 || contains_duplicate(guess) || !std::isdigit(guess[0])) {
        std::cout << "올바른 3자리 중복 없는 숫자를 입력하세요: ";
        std::cin >> guess;
    }
}

int main(void) {
    std::cout << "=== 숫자 야구 리벤지 ===\n";
    std::cout << "컴퓨터가 0~9 사이의 중복되지 않은 3자릿수를 선택했습니다!\n";
    std::cout << "10회 이내에 맞추세요!\n\n";

    const std::string answer = generate_answer();
    std::vector<std::string> history;
    std::vector<std::string> results;

    int attempts = 0;
    const int max_attempts = 10;

    while (attempts < max_attempts) {
        std::string guess;
        get_guess(guess);
        ++attempts;

        int strike = 0, ball = 0;
        for (int i = 0; i < 3; ++i) {
            if (guess[i] == answer[i]) {
                ++strike;
            } else if (answer.find(guess[i]) != std::string::npos) {
                ++ball;
            }
        }

        std::cout << "결과: " << strike << "S " << ball << "B\n\n";
        history.push_back(guess);
        results.push_back(std::to_string(strike) + "S " + std::to_string(ball) + "B");

        if (strike == 3) {
            std::cout << "정답! " << attempts << "번 만에 맞추셨습니다!\n";
            break;
        }
    }

    if (attempts == max_attempts) {
        std::cout << "실패! 정답은 " << answer << "였습니다.\n";
    }

    // --- 전적 출력 ---
    std::cout << "\n=== 전적 ===\n";
    std::cout << "시도한 횟수: " << attempts << "/" << max_attempts << "\n";
    std::cout << "정답 여부: " << (attempts <= max_attempts && history.size() > 0 ? "승리" : "패배") << "\n\n";

    std::cout << "=== 시도 기록 ===\n";
    for (size_t i = 0; i < history.size(); ++i) {
        std::cout << (i + 1) << "회: " << history[i] << " → " << results[i] << "\n";
    }

    return 0;
}
