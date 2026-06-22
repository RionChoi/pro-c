// Session 43: C++23 표준 라이브러리 - expected/flat_map/mdspan 스타일 던전 검증 게임
#include <algorithm>
#include <charconv>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

template <typename T, typename E>
class Expected {
public:
    Expected(T value) : storage_(std::move(value)) {}
    Expected(E error) : storage_(std::move(error)) {}

    bool has_value() const { return std::holds_alternative<T>(storage_); }
    explicit operator bool() const { return has_value(); }
    const T& value() const { return std::get<T>(storage_); }
    const E& error() const { return std::get<E>(storage_); }

private:
    std::variant<T, E> storage_;
};

template <typename Key, typename Value>
class FlatMap {
public:
    void insert_or_assign(Key key, Value value) {
        auto it = std::lower_bound(entries_.begin(), entries_.end(), key,
            [](const auto& entry, const Key& target) {
                return entry.first < target;
            });
        if (it != entries_.end() && it->first == key) {
            it->second = std::move(value);
        } else {
            entries_.insert(it, {std::move(key), std::move(value)});
        }
    }

    Value* find(const Key& key) {
        auto it = std::lower_bound(entries_.begin(), entries_.end(), key,
            [](const auto& entry, const Key& target) {
                return entry.first < target;
            });
        if (it != entries_.end() && it->first == key) {
            return &it->second;
        }
        return nullptr;
    }

    const std::vector<std::pair<Key, Value>>& entries() const {
        return entries_;
    }

private:
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

Expected<int, std::string> parse_int(std::string_view text) {
    int value = 0;
    const char* first = text.data();
    const char* last = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(first, last, value);
    if (ec != std::errc{} || ptr != last) {
        return std::string{"invalid number: "} + std::string{text};
    }
    return value;
}

struct Player {
    std::size_t row;
    std::size_t col;
    int hp;
};

void print_map(const MdSpan2D<char>& map, const Player& player) {
    for (std::size_t r = 0; r < map.rows(); ++r) {
        for (std::size_t c = 0; c < map.cols(); ++c) {
            if (r == player.row && c == player.col) {
                std::cout << '@';
            } else {
                std::cout << map(r, c);
            }
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== C++23-style dungeon validator ===\n";

    FlatMap<std::string, int> config;
    for (std::string_view raw : {"width=5", "height=4", "hp=30"}) {
        const auto pos = raw.find('=');
        std::string key{raw.substr(0, pos)};
        auto value = parse_int(raw.substr(pos + 1));
        if (!value) {
            std::cout << "config error: " << value.error() << '\n';
            return 1;
        }
        config.insert_or_assign(key, value.value());
    }

    const int* width_ptr = config.find("width");
    const int* height_ptr = config.find("height");
    const int* hp_ptr = config.find("hp");
    if (!width_ptr || !height_ptr || !hp_ptr) {
        std::cout << "config error: missing required key\n";
        return 1;
    }

    const std::size_t width = static_cast<std::size_t>(*width_ptr);
    const std::size_t height = static_cast<std::size_t>(*height_ptr);
    std::vector<char> cells(width * height, '.');
    MdSpan2D<char> map(cells.data(), height, width);
    map(1, 2) = '#';
    map(2, 3) = 'T';

    Player player{0, 0, *hp_ptr};
    print_map(map, player);
    std::cout << "HP: " << player.hp << '\n';

    if (map(2, 3) == 'T') {
        std::cout << "Treasure detected at row=2 col=3\n";
    }
}
