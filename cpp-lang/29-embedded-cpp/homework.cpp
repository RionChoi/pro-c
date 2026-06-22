// GPIO 레지스터 시뮬레이터
// 임베디드 C++ 핵심 패턴: volatile 레지스터, 비트 조작, std::expected 오류 처리
//
// 컴파일: g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp -o homework

#include <cstdint>
#include <expected>
#include <iostream>
#include <string_view>

// ─── 타입 안전 하드웨어 레지스터 ─────────────────────────────────────────────

template <typename T>
class HwRegister {
    volatile T value_{};

public:
    void set(uint8_t bit) noexcept {
        value_ = static_cast<T>(value_ | static_cast<T>(1U << bit));
    }
    void clear(uint8_t bit) noexcept {
        value_ = static_cast<T>(value_ & static_cast<T>(~(1U << bit)));
    }
    void toggle(uint8_t bit) noexcept {
        value_ = static_cast<T>(value_ ^ static_cast<T>(1U << bit));
    }
    [[nodiscard]] bool test(uint8_t bit) const noexcept {
        return (static_cast<unsigned>(value_) >> bit) & 1U;
    }
    void write(T v) noexcept { value_ = v; }
    [[nodiscard]] T read() const noexcept { return value_; }
};

// ─── GPIO 오류 타입 ───────────────────────────────────────────────────────────

enum class GpioError : uint8_t {
    InvalidPin,
    NotOutput,
    NotInput,
};

[[nodiscard]] static std::string_view gpio_error_str(GpioError e) noexcept {
    switch (e) {
        case GpioError::InvalidPin: return "핀 번호 범위 초과";
        case GpioError::NotOutput:  return "입력 핀에 쓰기 불가";
        case GpioError::NotInput:   return "출력 핀 읽기 불가";
    }
    return "알 수 없는 오류";
}

// ─── GPIO 포트 (8핀) ──────────────────────────────────────────────────────────

class GpioPort {
    static constexpr uint8_t PINS = 8;
    HwRegister<uint8_t> dir_;   // 방향 레지스터: 1=출력, 0=입력
    HwRegister<uint8_t> data_;  // 데이터 레지스터
    HwRegister<uint8_t> pull_;  // 풀업 레지스터 (입력 핀 전용)

    [[nodiscard]] bool valid(uint8_t pin) const noexcept { return pin < PINS; }

public:
    // 핀 방향 설정
    void set_output(uint8_t pin) noexcept { if (valid(pin)) dir_.set(pin); }
    void set_input (uint8_t pin) noexcept { if (valid(pin)) dir_.clear(pin); }
    void set_pullup(uint8_t pin) noexcept { if (valid(pin)) pull_.set(pin); }

    // 입력 핀 값 시뮬레이션 (실제 하드웨어에서는 외부 신호)
    void inject(uint8_t pin, bool high) noexcept {
        if (valid(pin) && !dir_.test(pin)) {
            if (high) data_.set(pin); else data_.clear(pin);
        }
    }

    std::expected<void, GpioError> write(uint8_t pin, bool high) {
        if (!valid(pin))     return std::unexpected{GpioError::InvalidPin};
        if (!dir_.test(pin)) return std::unexpected{GpioError::NotOutput};
        if (high) data_.set(pin); else data_.clear(pin);
        return {};
    }

    [[nodiscard]] std::expected<bool, GpioError> read(uint8_t pin) const {
        if (!valid(pin))    return std::unexpected{GpioError::InvalidPin};
        if (dir_.test(pin)) return std::unexpected{GpioError::NotInput};
        return data_.test(pin);
    }

    std::expected<void, GpioError> toggle(uint8_t pin) {
        if (!valid(pin))     return std::unexpected{GpioError::InvalidPin};
        if (!dir_.test(pin)) return std::unexpected{GpioError::NotOutput};
        data_.toggle(pin);
        return {};
    }

    void dump(std::string_view label) const {
        std::cout << "[" << label << "]\n";
        std::cout << "  핀  :  7 6 5 4  3 2 1 0\n";
        std::cout << "  DIR : ";
        for (int i = 7; i >= 0; --i) {
            std::cout << " " << (dir_.test(static_cast<uint8_t>(i)) ? "O" : "I");
            if (i == 4) std::cout << " ";
        }
        std::cout << "   (O=출력 I=입력)\n";
        std::cout << "  DATA: ";
        for (int i = 7; i >= 0; --i) {
            std::cout << " " << (data_.test(static_cast<uint8_t>(i)) ? "1" : "0");
            if (i == 4) std::cout << " ";
        }
        std::cout << "\n";
    }
};

// ─── 오류 처리 헬퍼 ───────────────────────────────────────────────────────────

static void check(std::expected<void, GpioError>&& r, std::string_view op) {
    if (!r)
        std::cerr << "  오류 [" << op << "]: " << gpio_error_str(r.error()) << "\n";
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== GPIO 레지스터 시뮬레이터 ===\n\n";

    GpioPort portA;

    // 방향 설정
    std::cout << "── 핀 설정: 0~3 출력, 4~7 입력 ──\n";
    for (uint8_t i = 0; i < 4; ++i) portA.set_output(i);
    for (uint8_t i = 4; i < 8; ++i) portA.set_input(i);
    portA.dump("초기 상태");

    // 출력 핀 조작
    std::cout << "\n── 출력 핀 쓰기 ──\n";
    portA.write(0, true);
    portA.write(1, false);
    portA.write(2, true);
    portA.write(3, true);
    portA.dump("0=H, 1=L, 2=H, 3=H");

    // 토글
    std::cout << "\n── 핀 0, 2 토글 ──\n";
    portA.toggle(0);
    portA.toggle(2);
    portA.dump("토글 후");

    // 입력 시뮬레이션
    std::cout << "\n── 입력 핀 시뮬레이션 ──\n";
    portA.inject(4, true);
    portA.inject(5, false);
    portA.inject(6, true);
    portA.inject(7, true);
    portA.dump("외부 신호 주입 후");

    std::cout << "\n입력 핀 읽기:\n";
    for (uint8_t i = 4; i < 8; ++i) {
        if (auto r = portA.read(i))
            std::cout << "  핀 " << static_cast<int>(i)
                      << " = " << (*r ? "HIGH" : "LOW") << "\n";
    }

    // 오류 처리 예시
    std::cout << "\n── 오류 처리 ──\n";
    check(portA.write(4, true), "write 입력핀");  // 입력 핀에 쓰기
    check(portA.write(9, true), "write 범위초과"); // 범위 초과
    check(portA.toggle(5),      "toggle 입력핀"); // 입력 핀 토글

    if (auto r = portA.read(0); !r)
        std::cerr << "  오류 [read 출력핀]: " << gpio_error_str(r.error()) << "\n";

    std::cout << "\n완료.\n";
    return 0;
}
