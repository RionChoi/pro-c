// 세션 33 — 게임: 수학 속성 퀴즈 (컴파일 타임 테이블 활용)
//
// 1~100 범위의 숫자에 대해 "소수인가?", "피보나치 수인가?" 등을
// 묻는 퀴즈 게임이다. 모든 판정 테이블은 컴파일 타임에 생성된다.
//
// 컴파일: g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp -o game1
// 엄격:   g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion
//         -Wnull-dereference -Wdouble-promotion game1.cpp -o game1

#include <iostream>
#include <array>
#include <string>
#include <cstdlib>
#include <ctime>

// ── 컴파일 타임 상수 ─────────────────────────────────────────────────────────

static constexpr int MAX_N  = 100;
static constexpr int ROUNDS = 7;

// ── 컴파일 타임 에라토스테네스의 체 ─────────────────────────────────────────

constexpr auto make_prime_sieve() noexcept {
    std::array<bool, MAX_N + 1> s{};
    s.fill(true);
    s[0] = s[1] = false;
    for (int i = 2; i * i <= MAX_N; ++i)
        if (s[static_cast<std::size_t>(i)])
            for (int j = i * i; j <= MAX_N; j += i)
                s[static_cast<std::size_t>(j)] = false;
    return s;
}

// ── 컴파일 타임 피보나치 집합 ────────────────────────────────────────────────

constexpr auto make_fib_flags() noexcept {
    std::array<bool, MAX_N + 1> f{};
    int a = 0, b = 1;
    while (a <= MAX_N) {
        f[static_cast<std::size_t>(a)] = true;
        int c = a + b;
        a = b;
        b = c;
    }
    return f;
}

// ── 컴파일 타임 완전제곱수 집합 ──────────────────────────────────────────────

constexpr auto make_square_flags() noexcept {
    std::array<bool, MAX_N + 1> f{};
    for (int i = 0; i * i <= MAX_N; ++i)
        f[static_cast<std::size_t>(i * i)] = true;
    return f;
}

// ── 컴파일 타임 약수 개수 테이블 ────────────────────────────────────────────

constexpr int divisor_count(int n) noexcept {
    int cnt = 0;
    for (int i = 1; i * i <= n; ++i)
        if (n % i == 0)
            cnt += (i == n / i) ? 1 : 2;
    return cnt;
}

constexpr auto make_div_counts() noexcept {
    std::array<int, MAX_N + 1> arr{};
    for (int i = 1; i <= MAX_N; ++i)
        arr[static_cast<std::size_t>(i)] = divisor_count(i);
    return arr;
}

// ── 컴파일 타임 완전수(perfect number) 집합 ──────────────────────────────────
// 완전수: 자신을 제외한 약수의 합 = 자신 (6, 28, 496 ...)

constexpr bool is_perfect(int n) noexcept {
    if (n < 2) return false;
    int s = 1;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            s += i;
            if (i != n / i) s += n / i;
        }
    }
    return s == n;
}

constexpr auto make_perfect_flags() noexcept {
    std::array<bool, MAX_N + 1> f{};
    for (int i = 2; i <= MAX_N; ++i)
        f[static_cast<std::size_t>(i)] = is_perfect(i);
    return f;
}

// ── 전역 컴파일 타임 테이블 ──────────────────────────────────────────────────

constexpr auto PRIMES   = make_prime_sieve();
constexpr auto FIBS     = make_fib_flags();
constexpr auto SQUARES  = make_square_flags();
constexpr auto DIV_CNT  = make_div_counts();
constexpr auto PERFECTS = make_perfect_flags();

// ── 컴파일 타임 검증 ─────────────────────────────────────────────────────────

static_assert(PRIMES[2]  && PRIMES[97]  && !PRIMES[100]);
static_assert(FIBS[0]    && FIBS[89]    && !FIBS[10]);
static_assert(SQUARES[49] && SQUARES[100] && !SQUARES[50]);
static_assert(DIV_CNT[12] == 6);
static_assert(PERFECTS[6] && PERFECTS[28] && !PERFECTS[12]);

// ── 문제 유형 ────────────────────────────────────────────────────────────────

enum class PropKind { PRIME, FIBONACCI, PERFECT_SQUARE, EVEN, PERFECT_NUM };

static constexpr int PROP_COUNT = 5;

const char* prop_label(PropKind k) {
    switch (k) {
        case PropKind::PRIME:          return "소수";
        case PropKind::FIBONACCI:      return "피보나치 수";
        case PropKind::PERFECT_SQUARE: return "완전제곱수";
        case PropKind::EVEN:           return "짝수";
        case PropKind::PERFECT_NUM:    return "완전수";
    }
    return "";
}

bool check_prop(int n, PropKind k) {
    switch (k) {
        case PropKind::PRIME:          return PRIMES[static_cast<std::size_t>(n)];
        case PropKind::FIBONACCI:      return FIBS[static_cast<std::size_t>(n)];
        case PropKind::PERFECT_SQUARE: return SQUARES[static_cast<std::size_t>(n)];
        case PropKind::EVEN:           return n % 2 == 0;
        case PropKind::PERFECT_NUM:    return PERFECTS[static_cast<std::size_t>(n)];
    }
    return false;
}

// ── 입력 헬퍼 ────────────────────────────────────────────────────────────────

bool get_yes_no() {
    std::string input;
    while (true) {
        std::cout << "  (y/n): ";
        if (!std::getline(std::cin, input)) return false;
        if (input == "y" || input == "Y") return true;
        if (input == "n" || input == "N") return false;
        std::cout << "  y 또는 n 으로 입력해 주세요.\n";
    }
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "============================================\n";
    std::cout << "  수학 속성 퀴즈 (constexpr 테이블 게임)\n";
    std::cout << "============================================\n";
    std::cout << "1~100 범위 숫자의 수학적 속성을 맞추세요.\n";
    std::cout << "(소수 체, 피보나치, 완전수 등 모든 테이블은\n";
    std::cout << " 컴파일 타임에 생성됩니다.)\n\n";

    int score = 0;

    for (int round = 1; round <= ROUNDS; ++round) {
        int number = 1 + std::rand() % MAX_N;
        PropKind prop = static_cast<PropKind>(std::rand() % PROP_COUNT);

        bool correct_answer = check_prop(number, prop);

        std::cout << "--- 문제 " << round << " / " << ROUNDS << " ---\n";
        std::cout << "  숫자 " << number
                  << " 은(는) " << prop_label(prop) << " 입니까?\n";

        bool user_answer = get_yes_no();

        if (user_answer == correct_answer) {
            std::cout << "  정답! (+1점)\n";
            ++score;
        } else {
            std::cout << "  틀렸습니다. 정답: "
                      << (correct_answer ? "예" : "아니오") << "\n";
        }

        std::cout << "  [참고] " << number << "의 약수 개수: "
                  << DIV_CNT[static_cast<std::size_t>(number)] << "\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "  최종 점수: " << score << " / " << ROUNDS << "\n";

    if (score == ROUNDS)
        std::cout << "  완벽합니다! 수학 감각이 뛰어나군요.\n";
    else if (score >= ROUNDS * 3 / 4)
        std::cout << "  훌륭합니다! 거의 다 맞췄어요.\n";
    else if (score >= ROUNDS / 2)
        std::cout << "  괜찮아요! 더 연습하면 완벽해질 거에요.\n";
    else
        std::cout << "  다시 도전해 보세요!\n";

    std::cout << "============================================\n";
    return 0;
}
