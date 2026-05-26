// 세션 31 과제 1: std::variant 도형 시스템
// Circle, Rectangle, Triangle을 variant로 묶고 std::visit으로 넓이·둘레를 계산한다.

#include <cmath>
#include <iostream>
#include <numbers>
#include <string>
#include <variant>
#include <vector>

struct Circle    { double radius; };
struct Rectangle { double width, height; };
struct Triangle  { double a, b, c; };

using Shape = std::variant<Circle, Rectangle, Triangle>;

struct AreaVisitor {
    double operator()(const Circle& c) const {
        return std::numbers::pi * c.radius * c.radius;
    }
    double operator()(const Rectangle& r) const {
        return r.width * r.height;
    }
    double operator()(const Triangle& t) const {
        double s = (t.a + t.b + t.c) / 2.0;
        return std::sqrt(s * (s - t.a) * (s - t.b) * (s - t.c));
    }
};

struct PerimeterVisitor {
    double operator()(const Circle& c) const {
        return 2.0 * std::numbers::pi * c.radius;
    }
    double operator()(const Rectangle& r) const {
        return 2.0 * (r.width + r.height);
    }
    double operator()(const Triangle& t) const {
        return t.a + t.b + t.c;
    }
};

std::string shapeName(const Shape& s) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, Circle>)    return "원";
        if constexpr (std::is_same_v<T, Rectangle>) return "직사각형";
        return "삼각형";
    }, s);
}

int main() {
    std::vector<Shape> shapes = {
        Circle{5.0},
        Rectangle{4.0, 6.0},
        Triangle{3.0, 4.0, 5.0},
        Circle{2.5},
        Rectangle{10.0, 3.0},
    };

    std::cout << "=== 도형 목록 ===\n";
    for (const auto& s : shapes) {
        double area  = std::visit(AreaVisitor{},      s);
        double peri  = std::visit(PerimeterVisitor{}, s);
        std::cout << shapeName(s)
                  << " — 넓이: " << area
                  << ", 둘레: "  << peri << "\n";
    }

    int circles = 0, rects = 0, tris = 0;
    double totalArea = 0.0;
    for (const auto& s : shapes) {
        totalArea += std::visit(AreaVisitor{}, s);
        if      (std::holds_alternative<Circle>(s))    ++circles;
        else if (std::holds_alternative<Rectangle>(s)) ++rects;
        else                                            ++tris;
    }

    std::cout << "\n전체 넓이 합계: " << totalArea << "\n";
    std::cout << "원: "       << circles
              << "개 | 직사각형: " << rects
              << "개 | 삼각형: "   << tris << "개\n";

    // std::get: 타입을 알고 있을 때 직접 꺼낸다 (틀리면 std::bad_variant_access)
    const auto& c = std::get<Circle>(shapes[0]);
    std::cout << "\n[std::get] 첫 번째 원 반지름: " << c.radius << "\n";

    // std::get_if: 포인터 반환 — 타입 불일치 시 nullptr
    if (const auto* r = std::get_if<Rectangle>(&shapes[1])) {
        std::cout << "[get_if]  두 번째 직사각형: "
                  << r->width << " × " << r->height << "\n";
    }

    return 0;
}
