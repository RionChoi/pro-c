#include <iostream>
#include <string>

template <typename T>
class Box {
public:
    explicit Box(const T& value) : value_(value) {}

    const T& get() const {
        return value_;
    }

private:
    T value_;
};

template <typename T>
double averageOfThree(const T& x, const T& y, const T& z) {
    return (static_cast<double>(x) + static_cast<double>(y) + static_cast<double>(z)) / 3.0;
}

int main() {
    Box<int> intBox(42);
    Box<std::string> strBox("C++ Template");

    std::cout << "intBox: " << intBox.get() << "\n";
    std::cout << "strBox: " << strBox.get() << "\n";

    double d1 = 0.0;
    double d2 = 0.0;
    double d3 = 0.0;

    std::cout << "실수 3개를 입력하세요: ";
    if (!(std::cin >> d1 >> d2 >> d3)) {
        std::cerr << "입력 오류: 실수를 올바르게 입력하세요.\n";
        return 1;
    }

    std::cout << "평균: " << averageOfThree(d1, d2, d3) << "\n";
    return 0;
}
