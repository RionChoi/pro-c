// 세션 33 — 과제 1: constexpr / consteval / constinit / if consteval 기초
// 컴파일: g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp -o hw33
// 엄격:   g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion
//         -Wnull-dereference -Wdouble-promotion homework.cpp -o hw33

#include <iostream>
#include <array>
#include <type_traits>
#include <cmath>

// ── 1. constexpr 수학 함수 ────────────────────────────────────────────────────

constexpr long long factorial(int n) {
    return (n <= 1) ? 1LL : static_cast<long long>(n) * factorial(n - 1);
}

constexpr long long power(long long base, int exp) {
    if (exp == 0) return 1LL;
    if (exp % 2 == 0) {
        long long half = power(base, exp / 2);
        return half * half;
    }
    return base * power(base, exp - 1);
}

constexpr bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

// ── 2. 컴파일 타임 피보나치 테이블 ───────────────────────────────────────────

constexpr auto make_fibonacci_table() {
    std::array<long long, 16> arr{};
    arr[0] = 0LL;
    arr[1] = 1LL;
    for (std::size_t i = 2; i < 16; ++i)
        arr[i] = arr[i - 1] + arr[i - 2];
    return arr;
}

// ── 3. consteval: 컴파일 타임 전용 즉시 함수 ─────────────────────────────────

consteval long long square_ct(long long n) { return n * n; }
consteval long long cube_ct(long long n)   { return n * n * n; }

consteval int next_power_of_two(int n) {
    int p = 1;
    while (p < n) p <<= 1;
    return p;
}

// ── 4. constinit: 전역 변수 컴파일 타임 초기화 보장 ──────────────────────────

constinit int g_max_capacity = 1024;

// ── 5. if constexpr: 타입 기반 컴파일 타임 분기 ──────────────────────────────

template<typename T>
constexpr const char* type_category() {
    if constexpr (std::is_integral_v<T>)
        return "정수형";
    else if constexpr (std::is_floating_point_v<T>)
        return "부동소수점형";
    else if constexpr (std::is_pointer_v<T>)
        return "포인터형";
    else
        return "기타";
}

template<typename T>
constexpr T absolute_val(T x) {
    if constexpr (std::is_unsigned_v<T>)
        return x;
    else
        return (x < T{0}) ? -x : x;
}

// ── 6. if consteval (C++23): 컴파일 타임 / 런타임 실행 경로 분리 ─────────────
//
// constexpr 함수가 컴파일 타임에 평가될 때는 표준 라이브러리(std::sqrt)를
// 호출할 수 없으므로, if consteval 블록에서 뉴턴-랩슨법으로 직접 계산한다.

constexpr double my_sqrt(double x) {
    if consteval {
        if (x == 0.0) return 0.0;
        double g = x;
        for (int i = 0; i < 60; ++i)
            g = 0.5 * (g + x / g);
        return g;
    } else {
        return std::sqrt(x);
    }
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
    // ── 컴파일 타임 검증 ──
    static_assert(factorial(10) == 3628800LL);
    static_assert(factorial(5)  == 120LL);
    static_assert(power(2LL, 10) == 1024LL);
    static_assert(power(3LL,  4) == 81LL);
    static_assert(is_prime(97)  == true);
    static_assert(is_prime(100) == false);

    static_assert(square_ct(7LL) == 49LL);
    static_assert(cube_ct(3LL)   == 27LL);
    static_assert(next_power_of_two(100) == 128);

    constexpr auto FIB = make_fibonacci_table();
    static_assert(FIB[10] == 55LL);
    static_assert(FIB[15] == 610LL);

    // ── 출력 ──
    std::cout << "=== 세션 33: constexpr / consteval / constinit 심화 ===\n\n";

    std::cout << "[ 1. constexpr 팩토리얼 ]\n";
    for (int i = 1; i <= 10; ++i)
        std::cout << "  " << i << "! = " << factorial(i) << "\n";

    std::cout << "\n[ 2. constexpr 거듭제곱 (2^n) ]\n";
    for (int i = 0; i <= 10; ++i)
        std::cout << "  2^" << i << " = " << power(2LL, i) << "\n";

    std::cout << "\n[ 3. 컴파일 타임 피보나치 테이블 ]\n";
    for (std::size_t i = 0; i < 16; ++i)
        std::cout << "  F(" << i << ") = " << FIB[i] << "\n";

    std::cout << "\n[ 4. consteval 즉시 함수 ]\n";
    constexpr long long s7 = square_ct(7LL);
    constexpr long long c3 = cube_ct(3LL);
    constexpr int       np = next_power_of_two(100);
    std::cout << "  7^2 = " << s7 << "\n";
    std::cout << "  3^3 = " << c3 << "\n";
    std::cout << "  100 이상 최소 2의 거듭제곱 = " << np << "\n";

    std::cout << "\n[ 5. if constexpr 타입 분기 ]\n";
    std::cout << "  int    : " << type_category<int>() << "\n";
    std::cout << "  double : " << type_category<double>() << "\n";
    std::cout << "  char*  : " << type_category<char*>() << "\n";
    std::cout << "  absolute_val(-42)  = " << absolute_val(-42) << "\n";
    std::cout << "  absolute_val(-3.14)= " << absolute_val(-3.14) << "\n";

    std::cout << "\n[ 6. constinit 전역 변수 ]\n";
    std::cout << "  g_max_capacity (컴파일 타임 초기화) = " << g_max_capacity << "\n";
    g_max_capacity = 4096;
    std::cout << "  런타임 수정 후 = " << g_max_capacity << "\n";

    std::cout << "\n[ 7. if consteval — 컴파일 타임 vs 런타임 sqrt ]\n";
    constexpr double ct_sqrt2 = my_sqrt(2.0);   // if consteval 분기: 뉴턴-랩슨
    double           rt_val   = 2.0;
    double           rt_sqrt2 = my_sqrt(rt_val); // else 분기: std::sqrt
    std::cout << "  컴파일 타임 sqrt(2) = " << ct_sqrt2 << "\n";
    std::cout << "  런타임    sqrt(2) = " << rt_sqrt2 << "\n";

    std::cout << "\n[ 8. 50 이하 소수 (is_prime — constexpr) ]\n  ";
    for (int n = 2; n <= 50; ++n)
        if (is_prime(n)) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}
