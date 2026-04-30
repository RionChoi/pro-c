#include <iostream>

class Rectangle {
private:
    double width;
    double height;

public:
    void setDimensions(double w, double h) {
        if (w <= 0 || h <= 0) {
            std::cerr << "오류: 너비와 높이는 0보다 커야 합니다.\n";
            width = 1;
            height = 1;
            return;
        }
        width = w;
        height = h;
    }

    double area() const {
        return width * height;
    }

    double perimeter() const {
        return 2.0 * (width + height);
    }

    bool isSquare() const {
        return width == height;
    }

    void print() const {
        std::cout << "직사각형 [너비: " << width << ", 높이: " << height << "] ";
        if (isSquare()) {
            std::cout << "→ 정사각형 O";
        } else {
            std::cout << "→ 정사각형 X";
        }
        std::cout << "\n";
        std::cout << "  면적: " << area() << ", 둘레: " << perimeter() << "\n";
    }
};

int main() {
    std::cout << "=== 직사각형 클래스 테스트 ===\n\n";

    Rectangle rects[3];

    rects[0].setDimensions(5.0, 3.0);
    rects[1].setDimensions(4.0, 4.0);
    rects[2].setDimensions(6.5, 2.3);

    for (int i = 0; i < 3; ++i) {
        std::cout << "rects[" << i << "] ";
        rects[i].print();
        std::cout << "\n";
    }

    return 0;
}
