#include <iostream>

void swap_ptr(int* a, int* b) {
    if (!a || !b) return;
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_ref(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}

int main(void) {
    std::cout << "=== 포인터 & 참조 과제 1 ===\n\n";

    int x, y;
    std::cout << "첫 번째 정수: ";
    if (!(std::cin >> x)) { std::cerr << "입력 오류\n"; return 1; }
    std::cout << "두 번째 정수: ";
    if (!(std::cin >> y)) { std::cerr << "입력 오류\n"; return 1; }

    // --- 포인터 버전 ---
    int a1 = x, b1 = y;
    std::cout << "\n[포인터 버전] 교환 전: a1=" << a1 << ", b1=" << b1 << "\n";
    swap_ptr(&a1, &b1);
    std::cout << "[포인터 버전] 교환 후: a1=" << a1 << ", b1=" << b1 << "\n";

    // --- 참조자 버전 ---
    int a2 = x, b2 = y;
    std::cout << "\n[참조자 버전] 교환 전: a2=" << a2 << ", b2=" << b2 << "\n";
    swap_ref(a2, b2);
    std::cout << "[참조자 버전] 교환 후: a2=" << a2 << ", b2=" << b2 << "\n";

    return 0;
}
