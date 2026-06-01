#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

// CRTP 베이스 클래스
template <typename Derived>
class Shape {
public:
    virtual ~Shape() = default;

    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }

    void display() const {
        std::cout << "넓이: " << area() << std::endl;
    }
};

// Circle 클래스
class Circle : public Shape<Circle> {
private:
    double radius;

public:
    Circle(double r) : radius(r) {}

    double area_impl() const {
        return M_PI * radius * radius;
    }

    double get_radius() const { return radius; }
};

// Rectangle 클래스
class Rectangle : public Shape<Rectangle> {
private:
    double width, height;

public:
    Rectangle(double w, double h) : width(w), height(h) {}

    double area_impl() const {
        return width * height;
    }

    double get_width() const { return width; }
    double get_height() const { return height; }
};

// is_shape Trait - CRTP 기반 타입 검증
template <typename T>
struct is_shape {
    static constexpr bool value = false;
};

template <typename T>
struct is_shape<Shape<T>> {
    static constexpr bool value = true;
};

// 제약 함수: Shape 타입만 받음
template <typename T>
typename std::enable_if<is_shape<T>::value, double>::type
calculate_total_area(const std::vector<std::shared_ptr<T>>& shapes) {
    double total = 0;
    for (const auto& shape : shapes) {
        total += shape->area();
    }
    return total;
}

// 다형성 벡터를 위한 헬퍼 함수
double calculate_mixed_area(const std::vector<std::shared_ptr<Shape<Circle>>>& circles,
                            const std::vector<std::shared_ptr<Shape<Rectangle>>>& rectangles) {
    double total = 0;

    for (const auto& circle : circles) {
        total += circle->area();
    }

    for (const auto& rect : rectangles) {
        total += rect->area();
    }

    return total;
}

int main() {
    std::cout << "=== CRTP와 Traits 정적 다형성 ===" << std::endl << std::endl;

    // 1. Circle 테스트
    std::cout << "1. Circle 테스트:" << std::endl;
    Circle c1(5.0);
    std::cout << "  반지름 5.0인 원: ";
    c1.display();
    std::cout << std::endl;

    // 2. Rectangle 테스트
    std::cout << "2. Rectangle 테스트:" << std::endl;
    Rectangle r1(4.0, 6.0);
    std::cout << "  가로 4.0, 세로 6.0인 직사각형: ";
    r1.display();
    std::cout << std::endl;

    // 3. is_shape Trait 테스트
    std::cout << "3. is_shape Trait 테스트:" << std::endl;
    std::cout << "  Circle이 Shape인가? "
              << (is_shape<Shape<Circle>>::value ? "yes" : "no") << std::endl;
    std::cout << "  Rectangle이 Shape인가? "
              << (is_shape<Shape<Rectangle>>::value ? "yes" : "no") << std::endl;
    std::cout << "  int가 Shape인가? "
              << (is_shape<int>::value ? "yes" : "no") << std::endl << std::endl;

    // 4. 총 넓이 계산 (Circle)
    std::cout << "4. 총 넓이 계산 (Circle):" << std::endl;
    std::vector<std::shared_ptr<Shape<Circle>>> circles;
    circles.push_back(std::make_shared<Circle>(3.0));
    circles.push_back(std::make_shared<Circle>(4.0));
    circles.push_back(std::make_shared<Circle>(5.0));
    double circle_total = calculate_total_area(circles);
    std::cout << "  3개 원의 총 넓이: " << circle_total << std::endl << std::endl;

    // 5. 총 넓이 계산 (Rectangle)
    std::cout << "5. 총 넓이 계산 (Rectangle):" << std::endl;
    std::vector<std::shared_ptr<Shape<Rectangle>>> rectangles;
    rectangles.push_back(std::make_shared<Rectangle>(2.0, 3.0));
    rectangles.push_back(std::make_shared<Rectangle>(4.0, 5.0));
    double rect_total = calculate_total_area(rectangles);
    std::cout << "  2개 직사각형의 총 넓이: " << rect_total << std::endl << std::endl;

    // 6. 혼합 총 넓이
    std::cout << "6. 혼합 도형의 총 넓이:" << std::endl;
    double mixed_total = calculate_mixed_area(circles, rectangles);
    std::cout << "  원 3개 + 직사각형 2개의 총 넓이: " << mixed_total << std::endl;

    return 0;
}
