#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <random>

// Forward declaration of the factory function from homework2.cpp
std::unique_ptr<class Shape> createShape(const std::string& type, double param1, double param2 = 0.0);

// Simplified Shape class definition required for this file
class Shape {
public:
    virtual ~Shape() = default;
    virtual double getArea() const = 0;
    virtual std::string getName() const = 0;
};

// Concrete Products (Must match those in homework2.cpp)
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


void runGame(int numRounds) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> type_dist(0, 1); // 0 for Circle, 1 for Rectangle
    std::uniform_real_distribution<> size_dist(1.0, 20.0);

    std::cout << "\n==========================================" << std::endl;
    std::cout << "        Shape Factory Game Starting!      " << std::endl;
    std::cout << "==========================================" << std::endl;

    double totalArea = 0.0;

    for (int i = 0; i < numRounds; ++i) {
        std::string type = (type_dist(gen) == 0) ? "Circle" : "Rectangle";
        double size1 = size_dist(gen);
        double size2 = (type == "Rectangle") ? size_dist(gen) : 0.0;

        // Use Factory Method dynamically
        std::unique_ptr<Shape> shape = createShape(type, size1, size2);

        if (shape) {
            std::cout << "Round " << i + 1 << ": Created a " << shape->getName() << " with area " << shape->getArea() << "." << std::endl;
            totalArea += shape->getArea();
        }
    }

    std::cout << "\n--- Game Over ---" << std::endl;
    std::cout << "Total calculated area across " << numRounds << " rounds: " << totalArea << std::endl;
}

int main() {
    // Note: In a real build system, we'd ensure createShape is visible. 
    // For this self-contained test, we'll rely on the simple implementation.
    runGame(5);
    return 0;
}