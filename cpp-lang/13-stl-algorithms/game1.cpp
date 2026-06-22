#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// 학생 구조체
struct Student {
    std::string name;
    int score;

    Student(const std::string& n, int s) : name(n), score(s) {}

    void display() const {
        std::cout << name << ": " << score << "점\n";
    }
};

int main() {
    // 학습: 실제 점수 관리 시스템 (std::sort, std::transform, std::find 활용)

    std::cout << "=== 점수 관리 시스템 (STL 알고리즘 활용) ===\n\n";

    // 학생 데이터
    std::vector<Student> students = {
        Student("Alice", 85),
        Student("Bob", 92),
        Student("Charlie", 78),
        Student("David", 95),
        Student("Eve", 88),
        Student("Frank", 80)
    };

    std::cout << "원본 학생 목록:\n";
    for (const auto& student : students) {
        student.display();
    }
    std::cout << "\n";

    // 1. 점수 기준 내림차순 정렬
    std::vector<Student> ranked = students;
    std::sort(ranked.begin(), ranked.end(),
              [](const Student& a, const Student& b) {
                  return a.score > b.score;
              });

    std::cout << "점수 순위 (내림차순):\n";
    for (size_t i = 0; i < ranked.size(); ++i) {
        std::cout << (i + 1) << "등. ";
        ranked[i].display();
    }
    std::cout << "\n";

    // 2. 특정 학생 찾기
    std::string target_name = "Charlie";
    auto it = std::find_if(students.begin(), students.end(),
                           [&target_name](const Student& s) {
                               return s.name == target_name;
                           });

    if (it != students.end()) {
        std::cout << target_name << "의 점수: " << it->score << "점\n\n";
    }

    // 3. 80점 이상인 학생 필터링
    std::vector<Student> passed;
    std::copy_if(students.begin(), students.end(), std::back_inserter(passed),
                 [](const Student& s) { return s.score >= 80; });

    std::cout << "80점 이상 합격자:\n";
    for (const auto& student : passed) {
        student.display();
    }
    std::cout << "\n";

    // 4. 학생 점수에 보너스 10점 추가 (transform 활용)
    std::vector<Student> with_bonus = students;
    std::transform(with_bonus.begin(), with_bonus.end(), with_bonus.begin(),
                   [](Student s) {
                       s.score = std::min(100, s.score + 10);  // 최대 100점
                       return s;
                   });

    std::cout << "보너스 10점 추가 후:\n";
    for (const auto& student : with_bonus) {
        student.display();
    }
    std::cout << "\n";

    // 5. 평균 점수 계산
    int total = 0;
    for (const auto& student : students) {
        total += student.score;
    }
    double average = static_cast<double>(total) / students.size();
    std::cout << "평균 점수: " << average << "점\n";

    // 6. 평균 이상인 학생 수
    int above_average = std::count_if(students.begin(), students.end(),
                                       [average](const Student& s) {
                                           return s.score >= average;
                                       });
    std::cout << "평균 이상 학생 수: " << above_average << "명\n";

    return 0;
}
