// 세션 21 — 미니 게임: 숫자 필터 퀴즈
// 플레이어가 Ranges 파이프라인 결과를 맞추는 퀴즈 게임
// 적용 개념: Concepts, std::views 파이프라인, ranges 알고리즘
#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <random>
#include <concepts>
#include <string>
#include <limits>

// ── Concepts ─────────────────────────────────────────────────────────────────
template <typename T>
concept Printable = requires(std::ostream& os, T v) {
    { os << v } -> std::same_as<std::ostream&>;
};

template <typename T>
concept Answerable = std::equality_comparable<T> && Printable<T>;

// ── 유틸리티 ─────────────────────────────────────────────────────────────────
static int inputInt(const std::string& prompt) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  숫자를 입력하세요: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}

template <std::ranges::range R>
void printRange(const std::string& label, R&& r) {
    std::cout << label << " [";
    bool first = true;
    for (auto&& v : r) {
        if (!first) std::cout << ", ";
        std::cout << v;
        first = false;
    }
    std::cout << "]\n";
}

// ── 퀴즈 문제 구조체 ──────────────────────────────────────────────────────────
struct Quiz {
    std::string description;
    std::vector<int> numbers;
    std::string operation;
    int answer;
};

// ── 퀴즈 생성 ────────────────────────────────────────────────────────────────
std::vector<Quiz> makeQuizzes(const std::vector<int>& base) {
    std::vector<Quiz> quizzes;

    // Q1: 짝수 개수
    {
        auto evens = base | std::views::filter([](int n){ return n % 2 == 0; });
        int cnt = static_cast<int>(std::ranges::distance(evens));
        quizzes.push_back({"위 숫자 중 짝수의 개수는?", base, "count_even", cnt});
    }

    // Q2: 5 초과 숫자들의 합
    {
        auto over5 = base | std::views::filter([](int n){ return n > 5; });
        int s = std::accumulate(over5.begin(), over5.end(), 0);
        quizzes.push_back({"위 숫자 중 5 초과인 것들의 합은?", base, "sum_over5", s});
    }

    // Q3: 최댓값의 제곱
    {
        int mx = *std::ranges::max_element(base);
        quizzes.push_back({"위 숫자 중 최댓값의 제곱은?", base, "max_square", mx * mx});
    }

    // Q4: 홀수 중 가장 큰 값
    {
        auto odds = base | std::views::filter([](int n){ return n % 2 != 0; });
        std::vector<int> oddVec(odds.begin(), odds.end());
        int maxOdd = *std::ranges::max_element(oddVec);
        quizzes.push_back({"위 숫자 중 홀수의 최댓값은?", base, "max_odd", maxOdd});
    }

    // Q5: 3의 배수 개수
    {
        auto mult3 = base | std::views::filter([](int n){ return n % 3 == 0; });
        int cnt = static_cast<int>(std::ranges::distance(mult3));
        quizzes.push_back({"위 숫자 중 3의 배수 개수는?", base, "count_mult3", cnt});
    }

    return quizzes;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════╗\n"
              << "║   🔢 Ranges 파이프라인 퀴즈 게임     ║\n"
              << "╚══════════════════════════════════════╝\n\n";

    std::cout << "C++20 Ranges 파이프라인 결과를 맞춰보세요!\n";
    std::cout << "총 5문제 — 각 정답당 20점\n\n";

    // 랜덤 숫자 생성
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 15);

    std::vector<int> numbers(10);
    std::ranges::generate(numbers, [&]{ return dist(rng); });

    std::cout << "주어진 숫자 배열:\n";
    printRange("  ", numbers);
    std::cout << "\n";

    auto quizzes = makeQuizzes(numbers);

    int score = 0;
    int qNum  = 1;

    for (const auto& q : quizzes) {
        std::cout << "── 문제 " << qNum++ << " ──────────────────────────\n";
        std::cout << "  " << q.description << "\n";

        int answer = inputInt("  정답: ");

        if (answer == q.answer) {
            std::cout << "  ✅ 정답! (+" << 20 << "점)\n\n";
            score += 20;
        } else {
            std::cout << "  ❌ 오답. 정답은 " << q.answer << "입니다.\n\n";
        }
    }

    // 결과
    std::cout << "══════════════════════════════════════\n";
    std::cout << "  최종 점수: " << score << " / 100\n";

    std::string grade;
    if      (score == 100) grade = "S — 완벽! Ranges 마스터!";
    else if (score >= 80)  grade = "A — 훌륭합니다!";
    else if (score >= 60)  grade = "B — 잘 했어요.";
    else if (score >= 40)  grade = "C — 조금 더 연습하세요.";
    else                   grade = "D — Ranges를 다시 복습해보세요.";

    std::cout << "  등급: " << grade << "\n";
    std::cout << "══════════════════════════════════════\n";

    return 0;
}
