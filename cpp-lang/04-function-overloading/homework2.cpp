#include <iostream>
#include <cmath>

double triangleArea(double base, double height) {
    if (base <= 0 || height <= 0) {
        std::cerr << "오류: 밑변과 높이는 양수여야 합니다.\n";
        return 0.0;
    }
    return 0.5 * base * height;
}

double circleArea(double radius) {
    if (radius <= 0) {
        std::cerr << "오류: 반지름은 양수여야 합니다.\n";
        return 0.0;
    }
    return M_PI * radius * radius;
}

double rectangleArea(double width, double height) {
    if (width <= 0 || height <= 0) {
        std::cerr << "오류: 가로와 세로는 양수여야 합니다.\n";
        return 0.0;
    }
    return width * height;
}

int main(void) {
    std::cout << "=== 도형 면적 계산기 ===\n\n";

    std::cout << "삼각형 (밑변: 6, 높이: 4): "
              << triangleArea(6.0, 4.0) << "\n";

    std::cout << "원 (반지름: 5): "
              << circleArea(5.0) << "\n";

    std::cout << "사각형 (가로: 7, 세로: 3): "
              << rectangleArea(7.0, 3.0) << "\n";

    std::cout << "\n유효하지 않은 입력 테스트:\n";
    triangleArea(-1.0, 5.0);
    circleArea(0.0);
    rectangleArea(3.0, -2.0);

    return 0;
}
