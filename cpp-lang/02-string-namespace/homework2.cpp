#include <cctype>
#include <iostream>
#include <string>

namespace textutil {

std::string toUpper(std::string text) {
    for (char &ch : text) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return text;
}

int countWords(const std::string &text) {
    int count = 0;
    bool inWord = false;

    for (char ch : text) {
        const bool isSpace = std::isspace(static_cast<unsigned char>(ch)) != 0;
        if (!isSpace && !inWord) {
            inWord = true;
            count++;
        } else if (isSpace) {
            inWord = false;
        }
    }

    return count;
}

}  // namespace textutil

int main() {
    std::string line;

    std::cout << "문장을 입력하세요: ";
    std::getline(std::cin, line);

    if (!std::cin || line.empty()) {
        std::cerr << "입력 오류: 비어 있지 않은 문장을 입력하세요.\n";
        return 1;
    }

    std::cout << "대문자 변환: " << textutil::toUpper(line) << "\n";
    std::cout << "단어 수: " << textutil::countWords(line) << "\n";

    return 0;
}
