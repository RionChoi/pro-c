#include <iostream>

char toGrade(double average) {
    if (average >= 90.0) {
        return 'A';
    }
    if (average >= 80.0) {
        return 'B';
    }
    if (average >= 70.0) {
        return 'C';
    }
    if (average >= 60.0) {
        return 'D';
    }
    return 'F';
}

int main() {
    int kor = 0;
    int eng = 0;
    int math = 0;

    std::cout << "국어 점수: ";
    if (!(std::cin >> kor)) {
        std::cerr << "입력 오류: 정수를 입력하세요.\n";
        return 1;
    }

    std::cout << "영어 점수: ";
    if (!(std::cin >> eng)) {
        std::cerr << "입력 오류: 정수를 입력하세요.\n";
        return 1;
    }

    std::cout << "수학 점수: ";
    if (!(std::cin >> math)) {
        std::cerr << "입력 오류: 정수를 입력하세요.\n";
        return 1;
    }

    if (kor < 0 || kor > 100 || eng < 0 || eng > 100 || math < 0 || math > 100) {
        std::cerr << "입력 오류: 점수는 0~100 사이여야 합니다.\n";
        return 1;
    }

    const int total = kor + eng + math;
    const double average = static_cast<double>(total) / 3.0;
    const char grade = toGrade(average);

    std::cout << "\n총점: " << total << "\n";
    std::cout << "평균: " << average << "\n";
    std::cout << "학점: " << grade << "\n";

    return 0;
}
