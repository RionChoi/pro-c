#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

// 함수 객체 (Functor) 클래스
struct Multiplier {
    int factor;
    
    Multiplier(int f) : factor(f) {}
    
    int operator()(int x) const {
        return x * factor;
    }
};

// 함수 객체 (Functor) - 누적 계산
struct Accumulator {
    int sum = 0;
    
    void operator()(int x) {
        sum += x;
    }
};

// 일반 함수 (std::function과 호환)
int add_ten(int x) {
    return x + 10;
}

int main() {
    std::cout << "=== 함수 객체 (Functor) ===\n\n";
    
    // 함수 객체 1: Multiplier
    Multiplier mul2(2);
    Multiplier mul5(5);
    
    std::cout << "10 * 2 = " << mul2(10) << "\n";
    std::cout << "10 * 5 = " << mul5(10) << "\n\n";
    
    // 함수 객체 2: 상태 유지
    std::cout << "=== Accumulator (상태 유지) ===\n";
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    Accumulator acc;
    for (int n : numbers) {
        acc(n);
    }
    std::cout << "합계: " << acc.sum << "\n\n";
    
    // std::function: 함수 포인터, 람다, 함수 객체 통합
    std::cout << "=== std::function 활용 ===\n";
    
    std::function<int(int)> func;
    
    // 1. 일반 함수 저장
    func = add_ten;
    std::cout << "add_ten(5) = " << func(5) << "\n";
    
    // 2. 람다 함수 저장
    func = [](int x) { return x * 3; };
    std::cout << "lambda x*3 (5) = " << func(5) << "\n";
    
    // 3. 함수 객체 저장
    func = Multiplier(7);
    std::cout << "Multiplier(7)(5) = " << func(5) << "\n\n";
    
    // std::function 벡터: 여러 연산 저장
    std::cout << "=== 함수 벡터 ===\n";
    std::vector<std::function<int(int)>> operations;
    operations.push_back([](int x) { return x * 2; });
    operations.push_back([](int x) { return x + 10; });
    operations.push_back([](int x) { return x * x; });
    operations.push_back(Multiplier(5));
    
    int value = 5;
    std::cout << "초기값: " << value << "\n";
    for (size_t i = 0; i < operations.size(); i++) {
        std::cout << "연산 " << (i + 1) << ": " << operations[i](value) << "\n";
    }
    
    std::cout << "\n";
    
    // 함수 객체를 벡터의 정렬에 사용
    std::cout << "=== 정렬에 함수 객체 사용 ===\n";
    std::vector<int> data = {5, 2, 8, 1, 9, 3};
    
    std::cout << "원본: ";
    for (int n : data) std::cout << n << " ";
    std::cout << "\n";
    
    // 커스텀 비교자
    struct DescendingOrder {
        bool operator()(int a, int b) const {
            return a > b;
        }
    };
    
    std::sort(data.begin(), data.end(), DescendingOrder());
    
    std::cout << "내림차순 정렬: ";
    for (int n : data) std::cout << n << " ";
    std::cout << "\n";
    
    return 0;
}
