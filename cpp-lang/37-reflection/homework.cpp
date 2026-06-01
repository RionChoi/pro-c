#include <iostream>
#include <vector>

template <typename Derived>
class Drawable {
public:
    void draw() {
        static_cast<Derived*>(this)->impl_draw();
    }

    void move(int dx, int dy) {
        static_cast<Derived*>(this)->impl_move(dx, dy);
    }
};

class Circle : public Drawable<Circle> {
    int x_, y_, radius_;

public:
    Circle(int x, int y, int r) : x_(x), y_(y), radius_(r) {}

    void impl_draw() {
        std::cout << "⭕ 원 그리기 (중심: " << x_ << "," << y_
                  << ", 반지름: " << radius_ << ")\n";
    }

    void impl_move(int dx, int dy) {
        x_ += dx;
        y_ += dy;
        std::cout << "원 이동 -> (" << x_ << "," << y_ << ")\n";
    }
};

class Rectangle : public Drawable<Rectangle> {
    int x_, y_, width_, height_;

public:
    Rectangle(int x, int y, int w, int h)
        : x_(x), y_(y), width_(w), height_(h) {}

    void impl_draw() {
        std::cout << "▭ 사각형 그리기 (위치: " << x_ << "," << y_
                  << ", 크기: " << width_ << "x" << height_ << ")\n";
    }

    void impl_move(int dx, int dy) {
        x_ += dx;
        y_ += dy;
        std::cout << "사각형 이동 -> (" << x_ << "," << y_ << ")\n";
    }
};

class Triangle : public Drawable<Triangle> {
    int x_, y_, size_;

public:
    Triangle(int x, int y, int s) : x_(x), y_(y), size_(s) {}

    void impl_draw() {
        std::cout << "△ 삼각형 그리기 (꼭짓점: " << x_ << "," << y_
                  << ", 크기: " << size_ << ")\n";
    }

    void impl_move(int dx, int dy) {
        x_ += dx;
        y_ += dy;
        std::cout << "삼각형 이동 -> (" << x_ << "," << y_ << ")\n";
    }
};

template <typename Shape>
class Canvas {
    std::vector<Shape> shapes_;

public:
    void add(const Shape& shape) {
        shapes_.push_back(shape);
    }

    void render_all() {
        std::cout << "\n=== 모든 도형 렌더링 ===\n";
        for (auto& shape : shapes_) {
            shape.draw();
        }
    }
};

int main() {
    Circle c(10, 20, 5);
    Rectangle r(0, 0, 30, 20);
    Triangle t(50, 50, 10);

    std::cout << "=== CRTP를 사용한 다형성 ===\n\n";

    c.draw();
    c.move(5, 5);

    r.draw();
    r.move(-2, 3);

    t.draw();
    t.move(10, -5);

    std::cout << "\n=== 도형별 캔바스 ===\n";
    Canvas<Circle> circle_canvas;
    circle_canvas.add(Circle(0, 0, 3));
    circle_canvas.add(Circle(15, 15, 7));
    circle_canvas.render_all();

    return 0;
}
