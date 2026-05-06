#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Abstract Product
class Shape {
public:
    virtual ~Shape() = default;
    virtual double getArea() const = 0;
    virtual std::string getName() const = 0;
};

// Concrete Products
class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double getArea() const override { return 3.14159 * radius * radius; }
    std::string getName() const override { return "Circle"; }
};

class Rectangle : public Shape {
private:
    double width;
    double height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double getArea() const override { return width * height; }
    std::string getName() const override { return "Rectangle"; }
};

// Simplified Factory Method
std::unique_ptr<Shape> createShape(const std::string& type, double param1, double param2 = 0.0) {
    if (type == "Circle") {
        return std::make_unique<Circle>(param1);
    } else if (type == "Rectangle") {
        return std::make_unique<Rectangle>(param1, param2);
    } else {
        return nullptr;
    }
}

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    
    // Use the Factory to create shapes
    shapes.push_back(createShape("Circle", 5.0));
    shapes.push_back(createShape("Rectangle", 4.0, 6.0));
    shapes.push_back(createShape("Circle", 10.0));

    std::cout << "--- Calculated Areas ---" << std::endl;
    for (const auto& shape : shapes) {
        if (shape) {
            std::cout << shape->getName() << " Area: " << shape->getArea() << std::endl;
        }
    }

    return 0;
}
