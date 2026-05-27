// 주사위 통계 대결 게임
// 컴파일: g++ -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic homework.cpp game1.cpp -o game1

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
import statistics;

static int roll(int sides) {
    return (std::rand() % sides) + 1;
}

static void print_rolls(const std::vector<double>& rolls) {
    std::cout << "  주사위: [";
    for (size_t i = 0; i < rolls.size(); ++i) {
        if (i) std::cout << ", ";
        std::cout << static_cast<int>(rolls[i]);
    }
    std::cout << "]\n";
}

static void print_bar(double val, double max_val) {
    int bars = (max_val > 0.0)
               ? static_cast<int>(val * 20.0 / max_val)
               : 0;
    std::cout << "  |";
    for (int i = 0; i < bars; ++i) std::cout << '#';
    std::cout << "  " << std::fixed << std::setprecision(1) << val << "\n";
}

static double play_round(int round_num, int num_dice, int sides) {
    std::cout << "\n[라운드 " << round_num << "] "
              << num_dice << "d" << sides << " 굴리기\n";

    std::vector<double> rolls;
    rolls.reserve(static_cast<size_t>(num_dice));
    for (int i = 0; i < num_dice; ++i)
        rolls.push_back(static_cast<double>(roll(sides)));

    print_rolls(rolls);

    double correct = Stats::mean(rolls);

    std::cout << "  평균을 맞춰보세요 (범위: 1 ~ " << sides << "): ";
    double guess = 0.0;
    if (!(std::cin >> guess)) {
        std::cerr << "입력 오류\n";
        return 0.0;
    }

    double diff = std::fabs(guess - correct);
    double points = 0.0;
    if (diff <= 0.25) {
        points = 3.0;
        std::cout << "  *** 완벽! (+3점)\n";
    } else if (diff <= 0.75) {
        points = 2.0;
        std::cout << "  **  근접! (+2점)\n";
    } else if (diff <= 1.5) {
        points = 1.0;
        std::cout << "  *   아깝다. (+1점)\n";
    } else {
        std::cout << "      너무 달라요. (0점)\n";
    }

    Stats::Summary s = Stats::summarize(rolls);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  정답 평균: " << correct
              << "  중앙값: " << s.median
              << "  표준편차: " << s.std_dev << "\n";

    return points;
}

static void show_final_stats(const std::vector<double>& scores) {
    Stats::Summary s = Stats::summarize(scores);
    double total = Stats::mean(scores) * static_cast<double>(scores.size());
    std::cout << "\n=== 최종 통계 ===\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  총점 합계  : " << total << "\n";
    std::cout << "  라운드 평균: " << s.mean << "\n";
    std::cout << "  최고 라운드: " << s.max_val << "점\n";
    std::cout << "  최저 라운드: " << s.min_val << "점\n";
    std::cout << "  편차       : " << s.std_dev << "\n";

    std::cout << "\n  라운드별 점수:\n";
    for (size_t i = 0; i < scores.size(); ++i) {
        std::cout << "  R" << (i + 1) << " ";
        print_bar(scores[i], 3.0);
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "====================================\n";
    std::cout << "  주사위 통계 대결 게임\n";
    std::cout << "  statistics 모듈 활용 예제\n";
    std::cout << "====================================\n\n";

    std::cout << "규칙: 주사위를 N번 굴린 결과의 평균을 맞추면 점수를 얻습니다.\n";
    std::cout << "  오차 <=0.25 -> 3점 | <=0.75 -> 2점 | <=1.5 -> 1점\n\n";

    const int total_rounds = 5;
    const int num_dice     = 6;
    const int sides        = 6;

    std::vector<double> round_scores;
    round_scores.reserve(total_rounds);

    for (int r = 1; r <= total_rounds; ++r) {
        double pts = play_round(r, num_dice, sides);
        round_scores.push_back(pts);
    }

    show_final_stats(round_scores);

    double total = Stats::mean(round_scores) * static_cast<double>(total_rounds);
    std::cout << "\n최종 점수: " << static_cast<int>(total) << " / "
              << (total_rounds * 3) << "점\n";

    if (total >= static_cast<double>(total_rounds) * 2.5)
        std::cout << "통계의 달인!\n";
    else if (total >= static_cast<double>(total_rounds) * 1.5)
        std::cout << "꽤 잘했어요!\n";
    else
        std::cout << "더 연습해 봐요!\n";

    return 0;
}
