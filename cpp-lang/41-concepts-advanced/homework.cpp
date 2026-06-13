// Session 41: C++20 Concepts 심화 — requires-expression & 커스텀 Concept 라이브러리
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>
#include <cmath>

// ─────────────────────────────────────────────
// [1] requires-expression 4가지 형태
// ─────────────────────────────────────────────

// simple requirement: 표현식이 단순히 유효한지
template <typename T>
concept Incrementable = requires(T t) {
    t++;   // simple: 후위 증감이 가능한가
    ++t;   // simple: 전위 증감이 가능한가
};

// type requirement: 중첩 타입이 존재하는지
template <typename T>
concept HasValueType = requires {
    typename T::value_type;  // type requirement
};

// compound requirement: 표현식 + 반환 타입
template <typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;  // compound
};

// nested requirement: 내부에서 다른 Concept 재확인
template <typename T>
concept SortableRange = requires(T c) {
    typename T::value_type;
    { c.begin() } -> std::input_or_output_iterator;
    { c.end()   } -> std::input_or_output_iterator;
    requires std::totally_ordered<typename T::value_type>;  // nested
};

void demo_requires_forms() {
    std::cout << "=== [1] requires-expression 4가지 형태 ===\n";

    std::cout << "Incrementable<int>:         " << Incrementable<int>         << '\n';
    std::cout << "Incrementable<double>:      " << Incrementable<double>      << '\n';
    std::cout << "Incrementable<std::string>: " << Incrementable<std::string> << '\n';

    std::cout << "HasValueType<std::vector<int>>: " << HasValueType<std::vector<int>> << '\n';
    std::cout << "HasValueType<int>:              " << HasValueType<int>              << '\n';

    std::cout << "Printable<int>:         " << Printable<int>         << '\n';
    std::cout << "Printable<std::string>: " << Printable<std::string> << '\n';

    std::cout << "SortableRange<std::vector<int>>:    " << SortableRange<std::vector<int>>    << '\n';
    std::cout << "SortableRange<std::list<double>>:   " << SortableRange<std::list<double>>   << '\n';
    std::cout << '\n';
}

// ─────────────────────────────────────────────
// [2] 커스텀 Concept 라이브러리
// ─────────────────────────────────────────────

// 수치 연산 가능한 타입
template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

// 덧셈/뺄셈/곱셈이 모두 가능한 타입
template <typename T>
concept Ring = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
};

// 나눗셈까지 지원하는 타입 (Field)
template <typename T>
concept Field = Ring<T> && requires(T a, T b) {
    { a / b } -> std::convertible_to<T>;
};

// 크기 비교 가능한 컨테이너
template <typename C>
concept SizedContainer = requires(C c) {
    typename C::value_type;
    { c.size()  } -> std::convertible_to<std::size_t>;
    { c.empty() } -> std::same_as<bool>;
    { c.begin() } -> std::input_or_output_iterator;
    { c.end()   } -> std::input_or_output_iterator;
};

// 인덱스 접근 가능한 컨테이너
template <typename C>
concept RandomAccessContainer = SizedContainer<C> && requires(C c, std::size_t i) {
    { c[i] } -> std::same_as<typename C::value_type&>;
};

// Concept 기반 제네릭 함수
template <Field T>
T safe_divide(T a, T b) {
    if (b == T{}) {
        std::cout << "  [경고] 0으로 나누기 방지, 0 반환\n";
        return T{};
    }
    return a / b;
}

template <SizedContainer C>
void print_container_info(const C& c) {
    std::cout << "  크기: " << c.size()
              << ", 비어있음: " << (c.empty() ? "예" : "아니오") << '\n';
}

template <RandomAccessContainer C>
typename C::value_type sum_elements(const C& c) {
    return std::accumulate(c.begin(), c.end(), typename C::value_type{});
}

void demo_concept_library() {
    std::cout << "=== [2] 커스텀 Concept 라이브러리 ===\n";

    std::cout << "Arithmetic<int>:    " << Arithmetic<int>    << '\n';
    std::cout << "Arithmetic<float>:  " << Arithmetic<float>  << '\n';
    std::cout << "Arithmetic<bool>:   " << Arithmetic<bool>   << '\n';
    std::cout << "Field<double>:      " << Field<double>       << '\n';
    std::cout << "Field<int>:         " << Field<int>          << '\n';

    std::cout << "\n  safe_divide(10.0, 3.0) = " << safe_divide(10.0, 3.0) << '\n';
    std::cout << "  safe_divide(5, 0): ";
    safe_divide(5, 0);

    std::vector<int> v{1, 2, 3, 4, 5};
    std::list<std::string> lst{"a", "b", "c"};

    std::cout << "\n  vector<int>:";
    print_container_info(v);
    std::cout << "  list<string>:";
    print_container_info(lst);

    std::cout << "  sum of vector = " << sum_elements(v) << '\n';
    std::cout << '\n';
}

// ─────────────────────────────────────────────
// [3] 중첩 제약 & 제약 결합
// ─────────────────────────────────────────────

template <typename T>
concept Serializable = requires(T t) {
    { t.serialize()   } -> std::same_as<std::string>;
    { T::deserialize(std::string{}) } -> std::same_as<T>;
};

template <typename T>
concept Hashable = requires(T t) {
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

// 둘 다 만족해야 캐시 가능
template <typename T>
concept Cacheable = Serializable<T> && Hashable<T>;

// 어느 하나만 만족해도 출력 가능
template <typename T>
concept Displayable = Printable<T> || Serializable<T>;

struct Point {
    double x, y;

    std::string serialize() const {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }
    static Point deserialize(const std::string& s) {
        // 간단한 파싱
        (void)s;
        return {0.0, 0.0};
    }
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << "Point(" << p.x << ", " << p.y << ")";
    }
};

// std::hash 특수화
namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point& p) const noexcept {
            auto h1 = std::hash<double>{}(p.x);
            auto h2 = std::hash<double>{}(p.y);
            return h1 ^ (h2 << 1);
        }
    };
}

void demo_nested_constraints() {
    std::cout << "=== [3] 중첩 제약 & 제약 결합 ===\n";

    std::cout << "Serializable<Point>: " << Serializable<Point> << '\n';
    std::cout << "Hashable<Point>:     " << Hashable<Point>     << '\n';
    std::cout << "Cacheable<Point>:    " << Cacheable<Point>    << '\n';
    std::cout << "Displayable<int>:    " << Displayable<int>    << '\n';
    std::cout << "Displayable<Point>:  " << Displayable<Point>  << '\n';

    Point p{3.14, 2.71};
    std::cout << "\n  serialize: " << p.serialize() << '\n';
    std::cout << "  hash: " << std::hash<Point>{}(p) << '\n';
    std::cout << "  print: " << p << '\n';
    std::cout << '\n';
}

// ─────────────────────────────────────────────
// [4] Concept 부분 순서 (Partial Ordering)
// ─────────────────────────────────────────────

template <typename T>
concept Animal = requires(T t) {
    { t.speak() } -> std::same_as<std::string>;
};

template <typename T>
concept Pet = Animal<T> && requires(T t) {
    { t.name() } -> std::same_as<std::string>;
};

template <typename T>
concept TrainedPet = Pet<T> && requires(T t, std::string_view cmd) {
    { t.execute(cmd) } -> std::same_as<bool>;
};

// 더 구체적인 Concept이 우선 선택됨
template <Animal T>
void describe(const T& t) {
    std::cout << "  [Animal] 소리: " << t.speak() << '\n';
}

template <Pet T>
void describe(const T& t) {
    std::cout << "  [Pet] 이름: " << t.name() << ", 소리: " << t.speak() << '\n';
}

template <TrainedPet T>
void describe(const T& t) {
    std::cout << "  [TrainedPet] 이름: " << t.name()
              << ", 소리: " << t.speak()
              << ", 훈련됨: true\n";
}

struct WildDog {
    std::string speak() const { return "으르렁!"; }
};

struct HomeDog {
    std::string speak() const { return "멍멍!"; }
    std::string name()  const { return "뭉치"; }
};

struct TrainedDog {
    std::string speak()                     const { return "왕왕!"; }
    std::string name()                      const { return "맥스"; }
    bool        execute(std::string_view c) const {
        std::cout << "      [" << c << " 실행!] ";
        return true;
    }
};

void demo_partial_ordering() {
    std::cout << "=== [4] Concept 부분 순서 (Partial Ordering) ===\n";

    WildDog  w;
    HomeDog  h;
    TrainedDog td;

    describe(w);
    describe(h);
    td.execute("앉아");
    describe(td);
    std::cout << '\n';
}

// ─────────────────────────────────────────────
// [5] 조건부 멤버 함수 & SFINAE 대체
// ─────────────────────────────────────────────

template <typename T>
class SmartBox {
    T value_;
public:
    explicit SmartBox(T v) : value_(std::move(v)) {}

    void print() const requires Printable<T> {
        std::cout << "  SmartBox: " << value_ << '\n';
    }

    T sqrt_val() const requires std::floating_point<T> {
        return static_cast<T>(std::sqrt(static_cast<double>(value_)));
    }

    bool is_positive() const requires Arithmetic<T> {
        return value_ > T{};
    }
};

void demo_conditional_members() {
    std::cout << "=== [5] 조건부 멤버 함수 ===\n";

    SmartBox<int>    ibox{42};
    SmartBox<double> dbox{2.0};
    SmartBox<std::string> sbox{"hello"};

    ibox.print();
    dbox.print();
    sbox.print();

    std::cout << "  sqrt(2.0) = " << dbox.sqrt_val() << '\n';
    std::cout << "  is_positive(42) = " << ibox.is_positive() << '\n';
    std::cout << '\n';
}

int main() {
    demo_requires_forms();
    demo_concept_library();
    demo_nested_constraints();
    demo_partial_ordering();
    demo_conditional_members();

    std::cout << "=== 세션 41 완료: C++20 Concepts 심화 ===\n";
    return 0;
}
