#include <iostream>
#include <cstring>

class MyString {
private:
    char* data;
    int length;

public:
    // 일반 생성자
    MyString(const char* str) {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
        std::cout << "[LOG] 생성자: 메모리 할당 (" << (void*)data << ")\n";
    }

    // 복사 생성자 (깊은 복사 구현)
    MyString(const MyString& other) {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
        std::cout << "[LOG] 복사 생성자: 깊은 복사 완료 (" << (void*)data << ")\n";
    }

    // 소멸자
    ~MyString() {
        if (data) {
            std::cout << "[LOG] 소멸자: 메모리 해제 (" << (void*)data << ")\n";
            delete[] data;
        }
    }

    void print() const {
        std::cout << "String content: " << data << " (Length: " << length << ")\n";
    }
};

int main() {
    std::cout << "=== 깊은 복사 vs 얕은 복사 테스트 ===\n\n";

    std::cout << "1. 원본 객체 생성\n";
    MyString s1("Hello, C++ World!");
    s1.print();

    std::cout << "\n2. 복사 생성자 호출 (s2 = s1)\n";
    MyString s2 = s1; // 복사 생성자 호출
    s2.print();

    std::cout << "\n3. 객체 소멸 시 메모리 안전성 확인\n";
    // s1과 s2가 각각 다른 메모리 주소를 가지므로 소멸 시 더블 프리 에러가 발생하지 않음.

    return 0;
}
