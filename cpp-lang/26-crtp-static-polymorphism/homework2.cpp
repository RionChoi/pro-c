// 세션 26: CRTP & 정적 다형성
// homework2.cpp — 정책 기반 컨테이너 + Singleton & Clone CRTP
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

// ==================================================
// Part 1: 정책 기반 설계 (Policy-based Design)
// 정렬 정책과 출력 정책을 컴파일 타임에 조합
// ==================================================

// 정렬 정책
template <typename Container>
struct AscendingSort {
    void apply_sort(Container& c) {
        std::sort(c.begin(), c.end());
    }
};

template <typename Container>
struct DescendingSort {
    void apply_sort(Container& c) {
        std::sort(c.begin(), c.end(),
                  std::greater<typename Container::value_type>{});
    }
};

// 출력 정책
template <typename Container>
struct CommaFormat {
    void apply_print(const Container& c) const {
        bool first = true;
        for (const auto& v : c) {
            if (!first) std::cout << ", ";
            std::cout << v;
            first = false;
        }
        std::cout << '\n';
    }
};

template <typename Container>
struct LineFormat {
    void apply_print(const Container& c) const {
        for (const auto& v : c)
            std::cout << "  " << v << '\n';
    }
};

// 정책 조합 컨테이너
template <
    typename T,
    template <typename> class SortPolicy  = AscendingSort,
    template <typename> class PrintPolicy = CommaFormat
>
class SortedList
    : private SortPolicy<std::vector<T>>
    , private PrintPolicy<std::vector<T>>
{
    std::vector<T> data_;
public:
    void push(const T& val) { data_.push_back(val); }

    void sort() {
        SortPolicy<std::vector<T>>::apply_sort(data_);
    }

    void print() const {
        PrintPolicy<std::vector<T>>::apply_print(data_);
    }

    std::size_t size() const { return data_.size(); }
};

// ==================================================
// Part 2: CRTP Singleton Mixin
// ==================================================

template <typename Derived>
class Singleton {
protected:
    Singleton()  = default;
    ~Singleton() = default;
public:
    static Derived& instance() {
        static Derived inst;
        return inst;
    }
    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&)                 = delete;
    Singleton& operator=(Singleton&&)      = delete;
};

class AppConfig : public Singleton<AppConfig> {
    friend class Singleton<AppConfig>;
    AppConfig() = default;

    std::string theme_{"dark"};
    int         font_size_{14};
public:
    const std::string& theme()     const { return theme_; }
    int                font_size() const { return font_size_; }
    void set_theme(std::string t)        { theme_     = std::move(t); }
    void set_font_size(int s)            { font_size_ = s; }
    void print() const {
        std::cout << "AppConfig { theme=" << theme_
                  << ", font_size=" << font_size_ << " }\n";
    }
};

// ==================================================
// Part 3: CRTP Clone Mixin
// ==================================================

template <typename Derived>
class Cloneable {
public:
    Derived clone() const {
        return *static_cast<const Derived*>(this);
    }
};

class Document : public Cloneable<Document> {
    std::string title_;
    std::string content_;
public:
    Document(std::string title, std::string content)
        : title_(std::move(title)), content_(std::move(content)) {}

    void set_content(std::string c) { content_ = std::move(c); }
    void print() const {
        std::cout << "[" << title_ << "] " << content_ << '\n';
    }
};

// ==================================================
// main
// ==================================================

int main() {
    // ── 정책 기반 컨테이너 ──
    std::cout << "=== 정책 기반 SortedList ===\n";

    SortedList<int> asc_comma;
    for (int v : std::vector<int>{5, 2, 8, 1, 9, 3})
        asc_comma.push(v);
    asc_comma.sort();
    std::cout << "오름차순 + 콤마:   ";
    asc_comma.print();

    SortedList<int, DescendingSort, LineFormat> desc_line;
    for (int v : std::vector<int>{5, 2, 8, 1, 9, 3})
        desc_line.push(v);
    desc_line.sort();
    std::cout << "내림차순 + 줄바꿈:\n";
    desc_line.print();

    SortedList<std::string> str_list;
    const char* words[] = {"banana", "apple", "cherry", "date"};
    for (const char* w : words)
        str_list.push(std::string(w));
    str_list.sort();
    std::cout << "문자열 오름차순 + 콤마: ";
    str_list.print();

    // ── Singleton CRTP ──
    std::cout << "\n=== Singleton CRTP ===\n";
    AppConfig::instance().print();

    AppConfig::instance().set_theme("light");
    AppConfig::instance().set_font_size(16);
    AppConfig::instance().print();

    AppConfig& ref1 = AppConfig::instance();
    AppConfig& ref2 = AppConfig::instance();
    std::cout << "같은 인스턴스? "
              << (&ref1 == &ref2 ? "예" : "아니오") << '\n';

    // ── Clone CRTP ──
    std::cout << "\n=== Clone CRTP ===\n";
    Document original{"보고서", "초안 내용입니다."};
    original.print();

    Document copy = original.clone();
    copy.set_content("수정된 최종본입니다.");

    std::cout << "원본: "; original.print();
    std::cout << "복사: "; copy.print();

    return 0;
}
