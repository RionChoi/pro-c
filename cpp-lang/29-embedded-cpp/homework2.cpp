// 고정소수점 산술 & RAII 임계 구역
// 임베디드 C++ 패턴: FPU 없이 실수 연산, 인터럽트 안전 접근
//
// 컴파일: g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o homework2

#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>

// ─── 고정소수점 수 (Q16.8 포맷) ───────────────────────────────────────────────
//
// int32_t 사용, 하위 8비트가 소수부 (정밀도: 1/256 ≈ 0.0039)
// 범위: -8388608.0 ~ +8388607.996

class Fixed {
    static constexpr int FRAC = 8;
    static constexpr int32_t SCALE = 1 << FRAC;  // 256

    int32_t raw_;

    explicit constexpr Fixed(int32_t raw, bool) noexcept : raw_{raw} {}

public:
    constexpr Fixed() noexcept : raw_{0} {}

    explicit constexpr Fixed(int v) noexcept
        : raw_{static_cast<int32_t>(v * SCALE)} {}

    explicit Fixed(double d) noexcept
        : raw_{static_cast<int32_t>(d * SCALE)} {}

    [[nodiscard]] static Fixed from_raw(int32_t r) noexcept {
        return Fixed{r, true};
    }
    [[nodiscard]] int32_t raw() const noexcept { return raw_; }

    [[nodiscard]] double to_double() const noexcept {
        return static_cast<double>(raw_) / SCALE;
    }

    // 산술 연산
    [[nodiscard]] Fixed operator+(Fixed rhs) const noexcept {
        return Fixed{raw_ + rhs.raw_, true};
    }
    [[nodiscard]] Fixed operator-(Fixed rhs) const noexcept {
        return Fixed{raw_ - rhs.raw_, true};
    }
    [[nodiscard]] Fixed operator*(Fixed rhs) const noexcept {
        // 곱셈 후 FRAC만큼 오른쪽 시프트해 스케일 유지
        int64_t tmp = static_cast<int64_t>(raw_) * rhs.raw_;
        return Fixed{static_cast<int32_t>(tmp >> FRAC), true};
    }
    [[nodiscard]] Fixed operator/(Fixed rhs) const noexcept {
        int64_t tmp = static_cast<int64_t>(raw_) << FRAC;
        return Fixed{static_cast<int32_t>(tmp / rhs.raw_), true};
    }
    [[nodiscard]] Fixed operator-() const noexcept {
        return Fixed{-raw_, true};
    }

    Fixed& operator+=(Fixed rhs) noexcept { raw_ += rhs.raw_; return *this; }
    Fixed& operator-=(Fixed rhs) noexcept { raw_ -= rhs.raw_; return *this; }
    Fixed& operator*=(Fixed rhs) noexcept { *this = *this * rhs; return *this; }

    [[nodiscard]] bool operator< (Fixed rhs) const noexcept { return raw_ < rhs.raw_; }
    [[nodiscard]] bool operator> (Fixed rhs) const noexcept { return raw_ > rhs.raw_; }
    [[nodiscard]] bool operator==(Fixed rhs) const noexcept { return raw_ == rhs.raw_; }
    [[nodiscard]] bool operator<=(Fixed rhs) const noexcept { return raw_ <= rhs.raw_; }
    [[nodiscard]] bool operator>=(Fixed rhs) const noexcept { return raw_ >= rhs.raw_; }

    friend std::ostream& operator<<(std::ostream& os, Fixed f) {
        return os << std::fixed << std::setprecision(4) << f.to_double();
    }
};

// ─── RAII 임계 구역 (인터럽트 비활성화 시뮬레이션) ────────────────────────────

class CriticalSection {
    static bool& irq_enabled() noexcept {
        static bool state = true;
        return state;
    }

    bool saved_;

public:
    CriticalSection() noexcept : saved_{irq_enabled()} {
        irq_enabled() = false;
        std::cout << "  [인터럽트 비활성화]\n";
    }
    ~CriticalSection() noexcept {
        irq_enabled() = saved_;
        std::cout << "  [인터럽트 복원: " << (saved_ ? "활성화" : "비활성화") << "]\n";
    }
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

    [[nodiscard]] static bool is_irq_enabled() noexcept { return irq_enabled(); }
};

// ─── PID 컨트롤러 (고정소수점 버전) ──────────────────────────────────────────

class PidController {
    Fixed kp_, ki_, kd_;
    Fixed integral_{};
    Fixed prev_error_{};

public:
    PidController(Fixed kp, Fixed ki, Fixed kd) noexcept
        : kp_{kp}, ki_{ki}, kd_{kd} {}

    Fixed compute(Fixed setpoint, Fixed measurement) noexcept {
        Fixed error = setpoint - measurement;
        integral_ += error;
        Fixed derivative = error - prev_error_;
        prev_error_ = error;
        return kp_ * error + ki_ * integral_ + kd_ * derivative;
    }

    void reset() noexcept {
        integral_   = Fixed{};
        prev_error_ = Fixed{};
    }
};

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== 고정소수점 & RAII 임계 구역 ===\n\n";

    // ── 고정소수점 산술 기초 ──────────────────────────────────────────────────
    std::cout << "── 고정소수점 산술 ──\n";
    Fixed a{3};
    Fixed b{Fixed(1.5)};
    std::cout << "  a = " << a << "  (raw=" << a.raw() << ")\n";
    std::cout << "  b = " << b << "  (raw=" << b.raw() << ")\n";
    std::cout << "  a + b = " << (a + b) << "\n";
    std::cout << "  a - b = " << (a - b) << "\n";
    std::cout << "  a * b = " << (a * b) << "\n";
    std::cout << "  a / b = " << (a / b) << "\n";

    // 근사 계산 비교
    Fixed pi{Fixed(3.14159)};
    Fixed r {Fixed(2.5)};
    Fixed area = pi * r * r;
    std::cout << "\n  원넓이 pi*r^2 (r=2.5): "  << area  << "\n";
    std::cout << "  실제값 (double):          "
              << std::fixed << std::setprecision(4)
              << (3.14159 * 2.5 * 2.5) << "\n";

    // ── RAII 임계 구역 ────────────────────────────────────────────────────────
    std::cout << "\n── RAII 임계 구역 ──\n";
    std::cout << "인터럽트 상태: " << (CriticalSection::is_irq_enabled() ? "활성" : "비활성") << "\n";
    {
        CriticalSection cs;
        std::cout << "  (임계 구역 내부)\n";
        std::cout << "  인터럽트 상태: " << (CriticalSection::is_irq_enabled() ? "활성" : "비활성") << "\n";
    }
    std::cout << "인터럽트 상태: " << (CriticalSection::is_irq_enabled() ? "활성" : "비활성") << "\n";

    // ── PID 컨트롤러 시뮬레이션 ───────────────────────────────────────────────
    std::cout << "\n── PID 컨트롤러 (온도 제어 시뮬레이션) ──\n";
    std::cout << "  목표 온도: 75.0°C, 초기 온도: 20.0°C\n\n";

    PidController pid{Fixed(0.8), Fixed(Fixed(0.1)), Fixed(Fixed(0.05))};
    Fixed setpoint{Fixed(75.0)};
    Fixed temperature{Fixed(20.0)};

    std::cout << std::setw(6) << "스텝"
              << std::setw(12) << "온도(°C)"
              << std::setw(12) << "출력(PWM)"
              << "\n";
    std::cout << std::string(30, '-') << "\n";

    for (int step = 0; step < 10; ++step) {
        Fixed output = pid.compute(setpoint, temperature);

        // 출력을 0~100% 클램핑
        Fixed zero{};
        Fixed hundred{Fixed(100.0)};
        if (output < zero)    output = zero;
        if (output > hundred) output = hundred;

        std::cout << std::setw(6) << step
                  << std::setw(12) << temperature
                  << std::setw(12) << output << "\n";

        // 온도 갱신: 출력의 20%가 온도 상승에 기여
        temperature += output * Fixed(Fixed(0.2));
    }

    std::cout << "\n완료.\n";
    return 0;
}
