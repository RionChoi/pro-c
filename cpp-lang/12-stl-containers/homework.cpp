#include <algorithm>
#include <deque>
#include <iostream>
#include <vector>

void printScores(const std::vector<int>& scores) {
    std::cout << "현재 점수: ";
    for (std::size_t i = 0; i < scores.size(); ++i) {
        std::cout << scores[i];
        if (i + 1 < scores.size()) {
            std::cout << ", ";
        }
    }
    std::cout << '\n';
}

int main() {
    std::vector<int> scores;
    std::deque<int> recentScores;

    std::cout << "점수 5개를 입력하세요: ";
    for (int i = 0; i < 5; ++i) {
        int value = 0;
        if (!(std::cin >> value)) {
            std::cerr << "입력 오류: 정수를 입력하세요.\n";
            return 1;
        }
        scores.push_back(value);
        recentScores.push_back(value);
    }

    printScores(scores);

    std::sort(scores.begin(), scores.end());
    std::cout << "정렬 후: ";
    printScores(scores);

    if (!recentScores.empty()) {
        std::cout << "최근 점수(앞): " << recentScores.front() << "\n";
        std::cout << "최근 점수(뒤): " << recentScores.back() << "\n";
    }

    return 0;
}
