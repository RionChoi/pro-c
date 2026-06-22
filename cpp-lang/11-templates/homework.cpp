#include <iostream>
#include <string>

template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}

template <typename T>
void printArray(const T arr[], int size) {
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i];
        if (i + 1 < size) {
            std::cout << ", ";
        }
    }
    std::cout << '\n';
}

int main() {
    int a = 0;
    int b = 0;
    std::cout << "정수 2개를 입력하세요: ";
    if (!(std::cin >> a >> b)) {
        std::cerr << "입력 오류: 정수를 올바르게 입력하세요.\n";
        return 1;
    }

    std::cout << "큰 값: " << maxValue(a, b) << "\n";

    double nums[5] = {1.2, 3.4, 2.2, 7.8, 4.5};
    std::cout << "double 배열 출력: ";
    printArray(nums, 5);

    std::string words[3] = {"template", "generic", "code"};
    std::cout << "string 배열 출력: ";
    printArray(words, 3);

    return 0;
}
