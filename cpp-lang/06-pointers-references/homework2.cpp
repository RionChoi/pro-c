#include <iostream>

int main(void) {
    std::cout << "=== 포인터 & 참조 과제 2 ===\n\n";

    int n;
    std::cout << "배열 크기: ";
    if (!(std::cin >> n) || n <= 0) { std::cerr << "입력 오류: 양수를 입력하세요.\n"; return 1; }

    // 동적 할당
    int* arr = new int[n]();
    std::cout << n << "개의 정수를 입력하세요:\n";
    for (int i = 0; i < n; ++i) {
        if (!(std::cin >> arr[i])) {
            std::cerr << "입력 오류.\n";
            delete[] arr;
            return 1;
        }
    }

    // 합계 & 평균 (포인터 산술 연산)
    long long sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += *(arr + i);
    }
    double avg = static_cast<double>(sum) / n;

    std::cout << "\n합계: " << sum << "\n";
    std::cout << "평균: " << avg << "\n";

    // 메모리 해제
    delete[] arr;
    arr = nullptr;

    // nullptr 체크
    std::cout << "\n메모리 해제 후 nullptr 체크: ";
    if (arr == nullptr) {
        std::cout << "정상 (포인터가 nullptr입니다)\n";
    } else {
        std::cout << "오류 (포인터가 유효합니다)\n";
    }

    return 0;
}
