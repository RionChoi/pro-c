#include <iostream>
#include <string>

int main(void) {
    int score;

    std::cout << "점수를 입력하세요 (0~100): ";
    if (!(std::cin >> score)) {
        std::cerr << "입력 오류: 숫자를 입력해야 합니다.\n";
        return 1;
    }

    if (score < 0 || score > 100) {
        std::cerr << "입력 오류: 0~100 사이의 값을 입력하세요.\n";
        return 1;
    }

    char grade;
    if (score >= 90) {
        grade = 'A';
    } else if (score >= 80) {
        grade = 'B';
    } else if (score >= 70) {
        grade = 'C';
    } else if (score >= 60) {
        grade = 'D';
    } else {
        grade = 'F';
    }

    std::cout << "학점: " << grade << "\n";
    return 0;
}
