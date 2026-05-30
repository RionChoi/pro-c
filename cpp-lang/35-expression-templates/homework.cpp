#include <iostream>
#include <vector>
#include <cstring>

const int SIZE = 1000;
int alloc_count = 0;

template <typename T>
class Vector {
public:
    T* data;
    int size;

    Vector(int n = 0) : size(n) {
        data = new T[n]();
        ++alloc_count;
    }

    ~Vector() {
        delete[] data;
    }

    Vector(const Vector& other) : size(other.size) {
        data = new T[size];
        std::copy(other.data, other.data + size, data);
        ++alloc_count;
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new T[size];
            std::copy(other.data, other.data + size, data);
            ++alloc_count;
        }
        return *this;
    }

    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int get_size() const { return size; }
};

template <typename T>
Vector<T> operator+(const Vector<T>& a, const Vector<T>& b) {
    Vector<T> result(a.get_size());
    for (int i = 0; i < a.get_size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

template <typename T, typename E>
class VectorExpr {
public:
    const E& expr;

    VectorExpr(const E& e) : expr(e) {}

    T operator[](int i) const {
        return expr[i];
    }

    int get_size() const {
        return expr.get_size();
    }
};

template <typename T, typename L, typename R>
class BinaryOp {
public:
    L left;
    R right;

    BinaryOp(const L& l, const R& r) : left(l), right(r) {}

    T operator[](int i) const {
        return left[i] + right[i];
    }

    int get_size() const {
        return left.get_size();
    }
};

template <typename T>
class ExprVector {
public:
    T* data;
    int size;

    ExprVector(int n = 0) : size(n) {
        data = new T[n]();
        ++alloc_count;
    }

    ~ExprVector() {
        delete[] data;
    }

    template <typename E>
    ExprVector(const VectorExpr<T, E>& expr) : size(expr.get_size()) {
        data = new T[size];
        ++alloc_count;
        for (int i = 0; i < size; ++i) {
            data[i] = expr[i];
        }
    }

    ExprVector(const ExprVector& other) : size(other.size) {
        data = new T[size];
        std::copy(other.data, other.data + size, data);
        ++alloc_count;
    }

    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int get_size() const { return size; }
};

template <typename T>
ExprVector<T> operator+(
    const ExprVector<T>& l,
    const ExprVector<T>& r) {
    ExprVector<T> result(l.get_size());
    for (int i = 0; i < l.get_size(); ++i) {
        result[i] = l[i] + r[i];
    }
    return result;
}

int main() {
    std::cout << "=== 나이브 벡터 (일반 연산) ===\n";
    alloc_count = 0;
    {
        Vector<double> a(SIZE), b(SIZE), c(SIZE);
        for (int i = 0; i < SIZE; ++i) {
            a[i] = i * 1.0;
            b[i] = i * 2.0;
            c[i] = i * 3.0;
        }

        Vector<double> result = a + b + c;
        std::cout << "result[0] = " << result[0]
                  << ", result[SIZE-1] = " << result[SIZE - 1] << "\n";
    }
    std::cout << "메모리 할당 횟수: " << alloc_count << "\n";
    std::cout << "a, b, c 생성(3) + (a+b) 임시(1) + ((a+b)+c) 임시(1) + result(1) = 6회\n\n";

    std::cout << "=== 표현식 템플릿 벡터 (최적화) ===\n";
    alloc_count = 0;
    {
        ExprVector<double> a(SIZE), b(SIZE), c(SIZE);
        for (int i = 0; i < SIZE; ++i) {
            a[i] = i * 1.0;
            b[i] = i * 2.0;
            c[i] = i * 3.0;
        }

        ExprVector<double> result = a + b + c;
        std::cout << "result[0] = " << result[0]
                  << ", result[SIZE-1] = " << result[SIZE - 1] << "\n";
    }
    std::cout << "메모리 할당 횟수: " << alloc_count << "\n";
    std::cout << "a, b, c 생성(3) + result(1) = 4회 (임시 객체 제거!)\n";

    return 0;
}
