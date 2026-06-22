#include <iostream>

int calculate(int a, int b) {
    return a + b;
}

int calculate(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        default:
            std::cerr << "지원하지 않는 연산자입니다.\n";
            return 0;
    }
}

double calculate(double a, double b) {
    return a + b;
}

int main(void) {
    std::cout << "=== 함수 오버로딩 데모 ===\n\n";

    int x = 10, y = 5;

    std::cout << "calculate(int, int):     "
              << x << " + " << y << " = " << calculate(x, y) << "\n";

    std::cout << "calculate(int, int, char): "
              << x << " - " << y << " = " << calculate(x, y, '-') << "\n";
    std::cout << "                          "
              << x << " * " << y << " = " << calculate(x, y, '*') << "\n";

    double dx = 3.5, dy = 2.1;
    std::cout << "calculate(double, double): "
              << dx << " + " << dy << " = " << calculate(dx, dy) << "\n";

    return 0;
}
