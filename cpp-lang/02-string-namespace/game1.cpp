#include <algorithm>
#include <iostream>
#include <string>

std::string reversedCopy(const std::string &word) {
    std::string result = word;
    std::reverse(result.begin(), result.end());
    return result;
}

int main() {
    const std::string answer = "namespace";
    const std::string hint = reversedCopy(answer);

    std::cout << "[단어 뒤집기 퀴즈]\n";
    std::cout << "힌트(뒤집힌 단어): " << hint << "\n";
    std::cout << "원래 단어를 입력하세요: ";

    std::string guess;
    std::getline(std::cin, guess);

    if (!std::cin || guess.empty()) {
        std::cerr << "입력 오류: 단어를 입력하세요.\n";
        return 1;
    }

    if (guess == answer) {
        std::cout << "정답! 문자열과 네임스페이스 학습 완료!\n";
    } else {
        std::cout << "오답! 정답은 '" << answer << "' 입니다.\n";
    }

    return 0;
}
