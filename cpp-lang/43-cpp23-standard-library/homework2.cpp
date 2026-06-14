// Session 43: C++23 표준 라이브러리 - flat_map, mdspan, print 스타일 fallback
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

template <typename Key, typename Value>
class FlatMap {
public:
    void insert_or_assign(Key key, Value value) {
        auto it = lower_bound(key);
        if (it != entries_.end() && it->first == key) {
            it->second = std::move(value);
            return;
        }
        entries_.insert(it, {std::move(key), std::move(value)});
    }

    const Value* find(const Key& key) const {
        auto it = lower_bound(key);
        if (it != entries_.end() && it->first == key) {
            return &it->second;
        }
        return nullptr;
    }

    const std::vector<std::pair<Key, Value>>& entries() const {
        return entries_;
    }

private:
    using Iterator = typename std::vector<std::pair<Key, Value>>::iterator;
    using ConstIterator = typename std::vector<std::pair<Key, Value>>::const_iterator;

    Iterator lower_bound(const Key& key) {
        return std::lower_bound(entries_.begin(), entries_.end(), key,
            [](const auto& entry, const Key& target) {
                return entry.first < target;
            });
    }

    ConstIterator lower_bound(const Key& key) const {
        return std::lower_bound(entries_.begin(), entries_.end(), key,
            [](const auto& entry, const Key& target) {
                return entry.first < target;
            });
    }

    std::vector<std::pair<Key, Value>> entries_;
};

template <typename T>
class MdSpan2D {
public:
    MdSpan2D(T* data, std::size_t rows, std::size_t cols)
        : data_(data), rows_(rows), cols_(cols) {}

    T& operator()(std::size_t row, std::size_t col) {
        return data_[row * cols_ + col];
    }

    const T& operator()(std::size_t row, std::size_t col) const {
        return data_[row * cols_ + col];
    }

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }

private:
    T* data_;
    std::size_t rows_;
    std::size_t cols_;
};

template <typename... Args>
void print_line(const Args&... args) {
    (std::cout << ... << args) << '\n';
}

int main() {
    print_line("=== flat_map-style inventory ===");
    FlatMap<std::string, int> stock;
    stock.insert_or_assign("potion", 8);
    stock.insert_or_assign("arrow", 30);
    stock.insert_or_assign("shield", 1);
    stock.insert_or_assign("potion", 10);

    for (const auto& [name, count] : stock.entries()) {
        print_line("  ", name, ": ", count);
    }

    if (const int* arrows = stock.find("arrow")) {
        print_line("arrow count = ", *arrows);
    }

    print_line("\n=== mdspan-style matrix view ===");
    std::vector<int> cells{
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    MdSpan2D<int> grid(cells.data(), 3, 3);
    grid(1, 1) = 99;

    for (std::size_t r = 0; r < grid.rows(); ++r) {
        for (std::size_t c = 0; c < grid.cols(); ++c) {
            std::cout << grid(r, c) << ' ';
        }
        std::cout << '\n';
    }
}
