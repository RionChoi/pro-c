#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

template<typename E>
class VecExpr {
public:
    double operator[](std::size_t i) const {
        return static_cast<const E&>(*this)[i];
    }

    std::size_t size() const {
        return static_cast<const E&>(*this).size();
    }
};

class Vec : public VecExpr<Vec> {
private:
    std::vector<double> data_;

public:
    Vec(std::size_t n) : data_(n) {}

    Vec(const Vec& other) : data_(other.data_) {}

    template<typename E>
    Vec(const VecExpr<E>& expr) {
        const E& e = static_cast<const E&>(expr);
        data_.resize(e.size());
        for (std::size_t i = 0; i < e.size(); ++i) {
            data_[i] = e[i];
        }
    }

    template<typename E>
    Vec& operator=(const VecExpr<E>& expr) {
        const E& e = static_cast<const E&>(expr);
        if (data_.size() != e.size()) {
            data_.resize(e.size());
        }
        for (std::size_t i = 0; i < e.size(); ++i) {
            data_[i] = e[i];
        }
        return *this;
    }

    double& operator[](std::size_t i) { return data_[i]; }
    double operator[](std::size_t i) const { return data_[i]; }
    std::size_t size() const { return data_.size(); }
};

template<typename L, typename R>
class VecAdd : public VecExpr<VecAdd<L, R>> {
private:
    const L& l_;
    const R& r_;

public:
    VecAdd(const L& l, const R& r) : l_(l), r_(r) {}

    double operator[](std::size_t i) const {
        return l_[i] + r_[i];
    }

    std::size_t size() const { return l_.size(); }
};

template<typename L, typename R>
class VecSub : public VecExpr<VecSub<L, R>> {
private:
    const L& l_;
    const R& r_;

public:
    VecSub(const L& l, const R& r) : l_(l), r_(r) {}

    double operator[](std::size_t i) const {
        return l_[i] - r_[i];
    }

    std::size_t size() const { return l_.size(); }
};

template<typename E>
class VecScale : public VecExpr<VecScale<E>> {
private:
    const E& e_;
    double scalar_;

public:
    VecScale(const E& e, double s) : e_(e), scalar_(s) {}

    double operator[](std::size_t i) const {
        return e_[i] * scalar_;
    }

    std::size_t size() const { return e_.size(); }
};

template<typename L, typename R>
VecAdd<L, R> operator+(const VecExpr<L>& l, const VecExpr<R>& r) {
    return VecAdd<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename L, typename R>
VecSub<L, R> operator-(const VecExpr<L>& l, const VecExpr<R>& r) {
    return VecSub<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename E>
VecScale<E> operator*(const VecExpr<E>& e, double s) {
    return VecScale<E>(static_cast<const E&>(e), s);
}

template<typename E>
VecScale<E> operator*(double s, const VecExpr<E>& e) {
    return VecScale<E>(static_cast<const E&>(e), s);
}

double naive_dot_product(const std::vector<double>& a, const std::vector<double>& b) {
    double result = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

double et_dot_product(const Vec& a, const Vec& b) {
    double result = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

int main() {
    std::cout << "=== 표현식 템플릿 성능 비교 ===\n\n";

    const std::size_t SIZE = 10000;
    const int ITERATIONS = 10000;

    std::cout << "1. 벡터 크기: " << SIZE << ", 반복: " << ITERATIONS << "\n\n";

    // 방법 1: 순진한 구현 (여러 임시 벡터)
    std::cout << "방법 1: 순진한 구현 (중간 벡터 생성)\n";
    std::cout << "연산: result = (a + b) * 2.0 - c\n\n";

    std::vector<double> a1(SIZE), b1(SIZE), c1(SIZE);
    for (std::size_t i = 0; i < SIZE; ++i) {
        a1[i] = i + 1.0;
        b1[i] = (i + 1.0) * 2.0;
        c1[i] = (i + 1.0) * 3.0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<double> result1(SIZE);
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        std::vector<double> temp1(SIZE);
        for (std::size_t i = 0; i < SIZE; ++i) {
            temp1[i] = a1[i] + b1[i];
        }

        std::vector<double> temp2(SIZE);
        for (std::size_t i = 0; i < SIZE; ++i) {
            temp2[i] = temp1[i] * 2.0;
        }

        for (std::size_t i = 0; i < SIZE; ++i) {
            result1[i] = temp2[i] - c1[i];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto naive_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "시간: " << naive_time << " ms\n";
    std::cout << "최종 결과[0]: " << result1[0] << "\n\n";

    // 방법 2: 표현식 템플릿 (임시 벡터 없음)
    std::cout << "방법 2: 표현식 템플릿 (임시 벡터 없음)\n";
    std::cout << "연산: result = a * 2.0 + b - c\n\n";

    Vec a2(SIZE), b2(SIZE), c2(SIZE);
    for (std::size_t i = 0; i < SIZE; ++i) {
        a2[i] = i + 1.0;
        b2[i] = (i + 1.0) * 2.0;
        c2[i] = (i + 1.0) * 3.0;
    }

    start = std::chrono::high_resolution_clock::now();
    Vec result2(SIZE);
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        result2 = a2 * 2.0 + b2 - c2;
    }
    end = std::chrono::high_resolution_clock::now();
    auto et_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "시간: " << et_time << " ms\n";
    std::cout << "최종 결과[0]: " << result2[0] << "\n\n";

    // 성능 비교
    std::cout << "=== 성능 분석 ===\n";
    std::cout << "순진한 구현: " << naive_time << " ms\n";
    std::cout << "표현식 템플릿: " << et_time << " ms\n";
    if (et_time > 0) {
        double speedup = static_cast<double>(naive_time) / static_cast<double>(et_time);
        std::cout << "속도 향상: " << speedup << "배\n";
    }

    std::cout << "\n=== 메모리 효율성 분석 ===\n";
    std::cout << "순진한 구현:\n";
    std::cout << "  - 임시 벡터 2개 생성: " << (SIZE * 2 * sizeof(double)) / 1024 << " KB\n";
    std::cout << "  - 각 반복마다 메모리 할당/해제 수행\n";
    std::cout << "\n표현식 템플릿:\n";
    std::cout << "  - 임시 벡터 0개 생성\n";
    std::cout << "  - 스택 메모리만 사용 (참조 저장)\n";

    std::cout << "\n=== 표현식 템플릿의 장점 ===\n";
    std::cout << "1. 메모리 효율성: 중간 결과를 저장하지 않음\n";
    std::cout << "2. 성능: 불필요한 메모리 할당/해제 제거\n";
    std::cout << "3. 캐시 효율: 한 번의 메모리 패스로 계산 완료\n";
    std::cout << "4. 컴파일러 최적화: 인라인 기회 증가\n";

    return 0;
}
