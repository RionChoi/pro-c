#include <iostream>
#include <iomanip>

int main(void) {
    int dan;

    std::cout << "구구단 출력 (1~9): ";
    if (!(std::cin >> dan)) {
        std::cerr << "입력 오류: 숫자를 입력해야 합니다.\n";
        return 1;
    }

    while (dan < 1 || dan > 9) {
        std::cout << "1~9 사이의 숫자를 입력하세요: ";
        if (!(std::cin >> dan)) {
            std::cerr << "입력 오류: 숫자를 입력해야 합니다.\n";
            return 1;
        }
    }

    std::cout << "\n=== " << dan << "단 ===\n";
    for (int i = 1; i <= 9; ++i) {
        std::cout << dan << " x " << std::setw(2) << i << " = "
                  << std::setw(2) << dan * i << "\n";
    }

    return 0;
}
