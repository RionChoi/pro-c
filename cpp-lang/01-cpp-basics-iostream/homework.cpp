#include <iostream>
#include <limits>
#include <string>

int main() {
    std::string name;
    int age = 0;
    double heightCm = 0.0;

    std::cout << "이름을 입력하세요: ";
    std::getline(std::cin, name);
    if (!std::cin || name.empty()) {
        std::cerr << "입력 오류: 이름을 올바르게 입력하세요.\n";
        return 1;
    }

    std::cout << "나이를 입력하세요: ";
    if (!(std::cin >> age)) {
        std::cerr << "입력 오류: 나이는 정수여야 합니다.\n";
        return 1;
    }

    std::cout << "키(cm)를 입력하세요: ";
    if (!(std::cin >> heightCm)) {
        std::cerr << "입력 오류: 키는 숫자여야 합니다.\n";
        return 1;
    }

    if (age <= 0 || heightCm <= 0.0) {
        std::cerr << "입력 오류: 나이와 키는 양수여야 합니다.\n";
        return 1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\n[자기소개]\n";
    std::cout << "안녕하세요, " << name << "님!\n";
    std::cout << "나이: " << age << "세\n";
    std::cout << "키: " << heightCm << "cm\n";

    return 0;
}
