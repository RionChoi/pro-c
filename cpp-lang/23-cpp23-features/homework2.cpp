// 세션 23 — 확장 과제: C++23 범위/언어 신기능 종합
// 적용 개념: views::zip, views::enumerate, if consteval, [[assume]],
//            다차원 operator[], string::contains, to_underlying, stdfloat
#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <utility>
#include <stdfloat>
#include <cmath>
#include <iomanip>

// ── 1. std::views::zip ───────────────────────────────────────────────────────
void demo_zip() {
    std::cout << "[1] std::views::zip\n";
    std::vector<std::string> names   = {"Alice", "Bob", "Charlie"};
    std::vector<int>         scores  = {92, 85, 78};
    std::vector<std::string> grades  = {"A", "B", "C"};

    std::cout << "  이름     | 점수 | 등급\n";
    std::cout << "  ---------+------+-----\n";
    for (auto [name, score, grade] : std::views::zip(names, scores, grades))
        std::cout << "  " << std::left
                  << std::setw(9) << name << "| "
                  << std::setw(5) << score << "| " << grade << "\n";

    // zip으로 내적(dot product) 계산
    std::vector<int> a = {1, 2, 3};
    std::vector<int> b = {4, 5, 6};
    int dot = 0;
    for (auto [x, y] : std::views::zip(a, b))
        dot += x * y;
    std::cout << "  내적 [1,2,3]·[4,5,6] = " << dot << "\n\n";
}

// ── 2. std::views::enumerate ─────────────────────────────────────────────────
void demo_enumerate() {
    std::cout << "[2] std::views::enumerate\n";
    std::vector<std::string> langs = {"C", "C++", "Python", "Rust", "Go"};

    for (auto [i, lang] : std::views::enumerate(langs))
        std::cout << "  [" << i << "] " << lang << "\n";

    // enumerate + filter: 홀수 인덱스만
    std::cout << "  홀수 인덱스: ";
    for (auto [i, lang] : std::views::enumerate(langs))
        if (i % 2 == 1) std::cout << lang << " ";
    std::cout << "\n\n";
}

// ── 3. if consteval ──────────────────────────────────────────────────────────
constexpr int compute(int n) {
    if consteval {
        // 컴파일 타임: 단순 계산만
        return n * n;
    } else {
        // 런타임: 더 복잡한 처리 가능
        return n * n + 1;
    }
}

void demo_if_consteval() {
    std::cout << "[3] if consteval\n";
    constexpr int ct = compute(5);  // 컴파일 타임: 25
    int           rt = compute(5);  // 런타임: 26
    std::cout << "  compile-time compute(5) = " << ct << "\n";
    std::cout << "  runtime   compute(5)    = " << rt << "\n\n";
}

// ── 4. [[assume]] ────────────────────────────────────────────────────────────
int safe_index(const std::vector<int>& v, std::size_t i) {
    [[assume(i < v.size())]];  // 컴파일러에 경계 조건 보장 힌트
    return v[i];
}

double fast_sqrt(double x) {
    [[assume(x >= 0.0)]];  // 음수가 오지 않음을 보장
    return std::sqrt(x);
}

void demo_assume() {
    std::cout << "[4] [[assume]]\n";
    std::vector<int> v = {10, 20, 30, 40, 50};
    std::cout << "  v[2] = " << safe_index(v, 2) << "\n";
    std::cout << "  fast_sqrt(16) = " << fast_sqrt(16.0) << "\n\n";
}

// ── 5. 다차원 operator[] ─────────────────────────────────────────────────────
struct Matrix3x3 {
    double data[3][3]{};

    // C++23: 다차원 subscript operator
    double& operator[](std::size_t r, std::size_t c) {
        return data[r][c];
    }
    const double& operator[](std::size_t r, std::size_t c) const {
        return data[r][c];
    }

    void print() const {
        for (int r = 0; r < 3; ++r) {
            std::cout << "  [";
            for (int c = 0; c < 3; ++c) {
                std::cout << std::setw(6) << data[r][c];
                if (c < 2) std::cout << ", ";
            }
            std::cout << "]\n";
        }
    }

    // 행렬 곱
    Matrix3x3 operator*(const Matrix3x3& o) const {
        Matrix3x3 result{};
        for (std::size_t i = 0; i < 3; ++i)
            for (std::size_t j = 0; j < 3; ++j)
                for (std::size_t k = 0; k < 3; ++k)
                    result[i, j] += (*this)[i, k] * o[k, j];
        return result;
    }
};

void demo_multidim_subscript() {
    std::cout << "[5] 다차원 operator[]\n";
    Matrix3x3 m;
    m[0, 0] = 1; m[0, 1] = 2; m[0, 2] = 3;
    m[1, 0] = 4; m[1, 1] = 5; m[1, 2] = 6;
    m[2, 0] = 7; m[2, 1] = 8; m[2, 2] = 9;

    std::cout << "  행렬 M:\n";
    m.print();

    // 단위 행렬
    Matrix3x3 ident;
    ident[0,0] = ident[1,1] = ident[2,2] = 1.0;

    auto product = m * ident;
    std::cout << "  M * 단위행렬:\n";
    product.print();
    std::cout << "\n";
}

// ── 6. std::string::contains ─────────────────────────────────────────────────
void demo_string_contains() {
    std::cout << "[6] std::string::contains\n";
    std::vector<std::string> words = {
        "hello world", "C++ programming", "modern cpp", "data science", "machine learning"
    };
    std::string kw = "c";
    std::cout << "  키워드 \"" << kw << "\" 포함 단어:\n";
    for (const auto& w : words)
        if (w.contains(kw))
            std::cout << "    - " << w << "\n";

    // starts_with / ends_with (C++20, 여기서도 확인)
    std::string filename = "report_2026.cpp";
    std::cout << "  \"" << filename << "\":\n";
    std::cout << "    starts_with(\"report\"): " << filename.starts_with("report") << "\n";
    std::cout << "    ends_with(\".cpp\"):     " << filename.ends_with(".cpp")    << "\n";
    std::cout << "    contains(\"2026\"):      " << filename.contains("2026")     << "\n\n";
}

// ── 7. std::to_underlying ────────────────────────────────────────────────────
enum class Color    { Red = 1, Green = 2, Blue = 4 };
enum class Status   { OK = 0, Warning = 1, Error = 2, Fatal = 3 };
enum class FileMode { Read = 0x1, Write = 0x2, Exec = 0x4 };

void demo_to_underlying() {
    std::cout << "[7] std::to_underlying\n";
    std::cout << "  Color::Red    = " << +std::to_underlying(Color::Red)    << "\n";
    std::cout << "  Color::Blue   = " << +std::to_underlying(Color::Blue)   << "\n";
    std::cout << "  Status::Error = " <<  std::to_underlying(Status::Error) << "\n";

    // 비트 플래그 조합
    auto mode = std::to_underlying(FileMode::Read) | std::to_underlying(FileMode::Write);
    std::cout << "  Read|Write    = 0x" << std::hex << mode << std::dec << "\n\n";
}

// ── 8. std::float16_t / float32_t ────────────────────────────────────────────
void demo_stdfloat() {
    std::cout << "[8] std::stdfloat (C++23)\n";
    std::float16_t f16 = static_cast<std::float16_t>(3.14);
    std::float32_t f32 = 3.14f;
    std::float64_t f64 = 3.14159265358979;

    std::cout << "  float16_t pi ≈ " << static_cast<float>(f16) << "\n";
    std::cout << "  float32_t pi ≈ " << f32 << "\n";
    std::cout << "  float64_t pi ≈ " << f64 << "\n";
    std::cout << "  sizeof: f16=" << sizeof(f16)
              << " f32=" << sizeof(f32)
              << " f64=" << sizeof(f64) << "\n\n";
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== C++23 신기능 종합 ===\n\n";
    demo_zip();
    demo_enumerate();
    demo_if_consteval();
    demo_assume();
    demo_multidim_subscript();
    demo_string_contains();
    demo_to_underlying();
    demo_stdfloat();
    return 0;
}
