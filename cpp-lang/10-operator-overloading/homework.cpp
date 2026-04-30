#include <iostream>

class Vec2 {
private:
    double x;
    double y;

public:
    Vec2(double xValue, double yValue) : x(xValue), y(yValue) {}

    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        os << '(' << v.x << ", " << v.y << ')';
        return os;
    }
};

int main() {
    const Vec2 a(1.5, 2.0);
    const Vec2 b(3.0, -1.0);
    const Vec2 c = a + b;

    std::cout << "a = " << a << '\n';
    std::cout << "b = " << b << '\n';
    std::cout << "a + b = " << c << '\n';
    std::cout << "(a + b) == c ? " << ((c == Vec2(4.5, 1.0)) ? "true" : "false") << '\n';
    return 0;
}
