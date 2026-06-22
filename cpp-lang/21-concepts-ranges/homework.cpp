// 세션 21 — 핵심 과제: Concepts
// 적용 개념: concept 정의, requires 절, 약식 함수 템플릿, 표준 concepts
#include <iostream>
#include <string>
#include <concepts>
#include <vector>

// ── 커스텀 Concepts ───────────────────────────────────────────────────────────

// 숫자 타입 (정수 또는 부동소수점)
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 출력 가능한 타입 (operator<< 지원)
template <typename T>
concept Printable = requires(std::ostream& os, T v) {
    { os << v } -> std::same_as<std::ostream&>;
};

// 비교 가능한 타입 (< 연산자 지원)
template <typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

// 컨테이너: begin()/end()/size() 지원
template <typename T>
concept Container = requires(T c) {
    { c.begin() } -> std::input_or_output_iterator;
    { c.end()   } -> std::input_or_output_iterator;
    { c.size()  } -> std::convertible_to<std::size_t>;
};

// ── Concept 제약 함수들 ───────────────────────────────────────────────────────

// requires 절 사용
template <typename T>
requires Numeric<T>
T square(T x) { return x * x; }

// 약식 함수 템플릿 (C++20)
Numeric auto cube(Numeric auto x) { return x * x * x; }

// 두 Comparable 값 중 최솟값
template <Comparable T>
T myMin(T a, T b) { return (a < b) ? a : b; }

// 두 Comparable 값 중 최댓값
template <Comparable T>
T myMax(T a, T b) { return (a < b) ? b : a; }

// Printable 타입 출력
template <Printable T>
void print(const std::string& label, T value) {
    std::cout << "  " << label << ": " << value << "\n";
}

// Container 크기 출력
template <Container C>
void printSize(const std::string& label, const C& c) {
    std::cout << "  " << label << " 크기: " << c.size() << "\n";
}

// ── 복합 Concept: 숫자 + 비교 가능 ───────────────────────────────────────────
template <typename T>
concept NumericComparable = Numeric<T> && Comparable<T>;

template <NumericComparable T>
T clamp(T value, T lo, T hi) {
    if (value < lo) return lo;
    if (hi < value) return hi;
    return value;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== Concepts 과제 ===\n\n";

    // 1. Numeric concept
    std::cout << "[1] Numeric concept\n";
    print("square(5)",     square(5));
    print("square(3.14)",  square(3.14));
    print("cube(4)",       cube(4));
    print("cube(2.5)",     cube(2.5));

    // 2. Comparable concept
    std::cout << "\n[2] Comparable concept\n";
    print("myMin(7, 3)",   myMin(7, 3));
    print("myMax(7, 3)",   myMax(7, 3));
    print("myMin(\"apple\", \"banana\")",
          myMin(std::string("apple"), std::string("banana")));

    // 3. Printable concept
    std::cout << "\n[3] Printable concept\n";
    print("정수",   42);
    print("실수",   3.14);
    print("문자열", std::string("hello"));

    // 4. Container concept
    std::cout << "\n[4] Container concept\n";
    std::vector<int>         vi = {1, 2, 3, 4, 5};
    std::vector<std::string> vs = {"a", "b", "c"};
    printSize("vector<int>",    vi);
    printSize("vector<string>", vs);

    // 5. NumericComparable concept — clamp
    std::cout << "\n[5] clamp (NumericComparable)\n";
    print("clamp(15, 0, 10)",  clamp(15, 0, 10));
    print("clamp(-5, 0, 10)",  clamp(-5, 0, 10));
    print("clamp(7, 0, 10)",   clamp(7, 0, 10));
    print("clamp(1.5, 0.0, 1.0)", clamp(1.5, 0.0, 1.0));

    // 6. 표준 라이브러리 Concepts 사용
    std::cout << "\n[6] 표준 Concepts 확인\n";
    std::cout << "  int is integral:        " << std::integral<int>        << "\n";
    std::cout << "  double is floating:     " << std::floating_point<double> << "\n";
    std::cout << "  string is integral:     " << std::integral<std::string> << "\n";
    std::cout << "  int is same as int:     " << std::same_as<int, int>    << "\n";
    std::cout << "  int derived from int:   " << std::derived_from<int,int> << "\n";

    return 0;
}
