// 컴파일: g++ -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic homework.cpp homework2.cpp -o hw2

#include <iostream>
#include <iomanip>
#include <vector>
import statistics;

struct Subject {
    const char* name;
    std::vector<double> scores;
};

static char to_grade(double score) {
    if (score >= 90.0) return 'A';
    if (score >= 80.0) return 'B';
    if (score >= 70.0) return 'C';
    if (score >= 60.0) return 'D';
    return 'F';
}

static void print_subject(const Subject& subj) {
    Stats::Summary s = Stats::summarize(subj.scores);
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "과목: " << subj.name << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  평균    : " << s.mean
              << "  [" << to_grade(s.mean) << "]\n";
    std::cout << "  중앙값  : " << s.median << "\n";
    std::cout << "  표준편차: " << s.std_dev << "\n";
    std::cout << "  최저점  : " << s.min_val
              << "  최고점: " << s.max_val << "\n";
}

static void grade_distribution(const std::vector<double>& scores) {
    int cnt[5] = {};
    for (double sc : scores) {
        char g = to_grade(sc);
        if (g == 'A')      ++cnt[0];
        else if (g == 'B') ++cnt[1];
        else if (g == 'C') ++cnt[2];
        else if (g == 'D') ++cnt[3];
        else               ++cnt[4];
    }
    const char labels[] = {'A', 'B', 'C', 'D', 'F'};
    std::cout << "  등급 분포: ";
    for (int i = 0; i < 5; ++i)
        if (cnt[i] > 0)
            std::cout << labels[i] << '\xc3' << '\x97' << cnt[i] << "  ";
    std::cout << "\n";
}

int main() {
    std::vector<Subject> subjects = {
        {"수학",   {85.0, 90.0, 78.0, 92.0, 88.0, 76.0, 95.0, 83.0}},
        {"영어",   {72.0, 68.0, 75.0, 80.0, 65.0, 88.0, 71.0, 79.0}},
        {"과학",   {91.0, 87.0, 93.0, 89.0, 95.0, 84.0, 90.0, 88.0}},
        {"역사",   {55.0, 62.0, 70.0, 48.0, 77.0, 59.0, 63.0, 45.0}},
    };

    std::cout << "=== 학생 성적 분석기 (statistics 모듈 사용) ===\n\n";

    for (const Subject& subj : subjects) {
        print_subject(subj);
        grade_distribution(subj.scores);
        std::cout << "\n";
    }

    // 전과목 통합 분석
    std::vector<double> all;
    for (const Subject& subj : subjects)
        all.insert(all.end(), subj.scores.begin(), subj.scores.end());

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "[ 전과목 통합 ]\n";
    Subject combined{"전과목", all};
    print_subject(combined);
    grade_distribution(all);

    return 0;
}
