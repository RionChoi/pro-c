#include <cstddef>
#include <iostream>
#include <string>

int main() {
    std::string sentence;

    std::cout << "문장을 입력하세요: ";
    std::getline(std::cin, sentence);

    if (!std::cin || sentence.empty()) {
        std::cerr << "입력 오류: 비어 있지 않은 문장을 입력하세요.\n";
        return 1;
    }

    std::size_t spaceCount = 0;
    for (char ch : sentence) {
        if (ch == ' ') {
            spaceCount++;
        }
    }

    std::cout << "\n[문자열 분석]\n";
    std::cout << "길이: " << sentence.size() << "\n";
    std::cout << "공백 수: " << spaceCount << "\n";
    std::cout << "첫 문자: " << sentence.front() << "\n";
    std::cout << "마지막 문자: " << sentence.back() << "\n";

    return 0;
}
