// 세션 26: CRTP & 정적 다형성
// homework.cpp — CRTP 도형 계층 + Comparable Mixin
#include <iostream>
#include <cmath>

static constexpr double PI = 3.14159265358979323846;

// ==================================================
// Part 1: CRTP 기반 도형 계층
// ==================================================

template <typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
    double perimeter() const {
        return static_cast<const Derived*>(this)->perimeter_impl();
    }
    const char* name() const {
        return static_cast<const Derived*>(this)->name_impl();
    }
    void describe() const {
        std::cout << name()
                  << " — 넓이: " << area()
                  << ", 둘레: " << perimeter() << '\n';
    }
};

class Circle : public Shape<Circle> {
    double r_;
public:
    explicit Circle(double r) : r_(r) {}
    double      area_impl()      const { return PI * r_ * r_; }
    double      perimeter_impl() const { return 2.0 * PI * r_; }
    const char* name_impl()      const { return "원"; }
};

class Rectangle : public Shape<Rectangle> {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double      area_impl()      const { return w_ * h_; }
    double      perimeter_impl() const { return 2.0 * (w_ + h_); }
    const char* name_impl()      const { return "직사각형"; }
};

// 세 변으로 넓이를 구하는 삼각형 (헤론의 공식)
class Triangle : public Shape<Triangle> {
    double a_, b_, c_;
public:
    Triangle(double a, double b, double c) : a_(a), b_(b), c_(c) {}
    double area_impl() const {
        double s = (a_ + b_ + c_) / 2.0;
        return std::sqrt(s * (s - a_) * (s - b_) * (s - c_));
    }
    double      perimeter_impl() const { return a_ + b_ + c_; }
    const char* name_impl()      const { return "삼각형"; }
};

// ==================================================
// Part 2: CRTP Mixin — Comparable
// compare_value() 하나만 구현하면 비교 연산자 전부 자동 생성
// ==================================================

template <typename Derived>
class Comparable {
    const Derived& self() const { return *static_cast<const Derived*>(this); }
public:
    bool operator< (const Derived& rhs) const { return self().compare_value() <  rhs.compare_value(); }
    bool operator> (const Derived& rhs) const { return rhs < self(); }
    bool operator<=(const Derived& rhs) const { return !(self() > rhs); }
    bool operator>=(const Derived& rhs) const { return !(self() < rhs); }
    // hidden friend: 비멤버로 정의해 C++20 역전 후보 중복을 방지
    friend bool operator==(const Derived& a, const Derived& b) {
        return !(a < b) && !(b < a);
    }
};

class Temperature : public Comparable<Temperature> {
    double celsius_;
public:
    explicit Temperature(double c) : celsius_(c) {}
    double compare_value() const { return celsius_; }
    void print() const { std::cout << celsius_ << "°C"; }
};

class Score : public Comparable<Score> {
    int points_;
public:
    explicit Score(int p) : points_(p) {}
    int  compare_value() const { return points_; }
    void print()         const { std::cout << points_ << "점"; }
};

// ==================================================
// main
// ==================================================

int main() {
    std::cout << "=== CRTP 도형 계층 ===\n";

    Circle    c{5.0};
    Rectangle r{4.0, 6.0};
    Triangle  t{3.0, 4.0, 5.0};

    c.describe();
    r.describe();
    t.describe();

    std::cout << "\n원 넓이 > 직사각형? "
              << (c.area() > r.area() ? "예" : "아니오") << '\n';
    std::cout << "직사각형 넓이 > 삼각형? "
              << (r.area() > t.area() ? "예" : "아니오") << '\n';

    std::cout << "\n=== vptr 없음 — 타입 크기 ===\n";
    std::cout << "Circle:    " << sizeof(Circle)    << " bytes\n";
    std::cout << "Rectangle: " << sizeof(Rectangle) << " bytes\n";
    std::cout << "Triangle:  " << sizeof(Triangle)  << " bytes\n";

    std::cout << "\n=== CRTP Mixin — Comparable<Temperature> ===\n";
    Temperature t1{100.0}, t2{-10.0}, t3{37.0};

    t1.print(); std::cout << " > "; t2.print();
    std::cout << " → " << (t1 > t2 ? "참" : "거짓") << '\n';

    t2.print(); std::cout << " < "; t3.print();
    std::cout << " → " << (t2 < t3 ? "참" : "거짓") << '\n';

    t1.print(); std::cout << " == "; t1.print();
    std::cout << " → " << (t1 == t1 ? "참" : "거짓") << '\n';

    std::cout << "\n=== CRTP Mixin — Comparable<Score> ===\n";
    Score s1{95}, s2{87}, s3{95};

    s1.print(); std::cout << " >= "; s2.print();
    std::cout << " → " << (s1 >= s2 ? "참" : "거짓") << '\n';

    s1.print(); std::cout << " == "; s3.print();
    std::cout << " → " << (s1 == s3 ? "참" : "거짓") << '\n';

    s2.print(); std::cout << " <= "; s1.print();
    std::cout << " → " << (s2 <= s1 ? "참" : "거짓") << '\n';

    return 0;
}
