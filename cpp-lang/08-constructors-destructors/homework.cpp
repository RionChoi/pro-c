#include <iostream>
#include <string>

class Student {
private:
    std::string name;
    int age;
    double score;

public:
    // 기본 생성자
    Student() : name("Unknown"), age(0), score(0.0) {
        std::cout << "[LOG] 기본 생성자 호출 (이름 미지정)\n";
    }

    // 매개변수 생성자 (초기화 리스트 사용)
    Student(std::string n, int a, double s) : name(n), age(a), score(s) {
        std::cout << "[LOG] 매개변수 생성자 호출 (이름: " << name << ")\n";
    }

    // 소멸자
    ~Student() {
        std::cout << "[LOG] 소멸자 호출 (학생: " << name << " 정보가 메모리에서 해제됩니다.)\n";
    }

    void display() const {
        std::cout << "  이름: " << name << ", 나이: " << age << ", 점수: " << score << "\n";
    }
};

int main() {
    std::cout << "=== 학생 관리 시스템 (생성자/소멸자 테스트) ===\n\n";

    {
        std::cout << "1. 블록 내에서 지역 객체 생성\n";
        Student s1("홍길동", 20, 95.5);
        s1.display();
        std::cout << "블록을 벗어납니다...\n";
    }

    std::cout << "\n2. 기본 생성자 객체 생성\n";
    Student s2;
    s2.display();

    std::cout << "\n3. 동적 할당 객체 생성\n";
    Student* s3 = new Student("임꺽정", 25, 88.0);
    s3->display();
    std::cout << "동적 객체를 삭제합니다...\n";
    delete s3;

    std::cout << "\n메인 함수 종료\n";
    return 0;
}
