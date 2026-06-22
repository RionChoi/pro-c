#include <cctype>
#include <iostream>
#include <map>
#include <string>

std::string normalizeWord(const std::string& word) {
    std::string result;
    result.reserve(word.size());
    for (char ch : word) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return result;
}

int main() {
    std::map<std::string, int> frequency;

    std::cout << "단어 6개를 입력하세요: ";
    for (int i = 0; i < 6; ++i) {
        std::string word;
        if (!(std::cin >> word)) {
            std::cerr << "입력 오류: 단어를 입력하세요.\n";
            return 1;
        }
        ++frequency[normalizeWord(word)];
    }

    std::cout << "\n단어 빈도 결과\n";
    for (const auto& entry : frequency) {
        std::cout << entry.first << " : " << entry.second << "\n";
    }

    return 0;
}
