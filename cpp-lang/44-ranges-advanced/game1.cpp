// Session 44: Ranges 심화 — 파이프라인 텍스트 처리 게임
// 플레이어가 단계별 range 변환을 선택해 단어 목록을 가공하는 미니 게임
#include <ranges>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <numeric>
#include <map>

// ─────────────────────────────────────────────
// 텍스트 유틸리티
// ─────────────────────────────────────────────

std::string to_upper(std::string s) {
    std::ranges::transform(s, s.begin(),
        [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
    return s;
}

std::string to_lower(std::string s) {
    std::ranges::transform(s, s.begin(),
        [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string reverse_str(std::string s) {
    std::ranges::reverse(s);
    return s;
}

bool has_vowel(const std::string& s) {
    return std::ranges::any_of(s, [](unsigned char c){
        std::string v = "aeiouAEIOU";
        return v.find(static_cast<char>(c)) != std::string::npos;
    });
}

// ─────────────────────────────────────────────
// 게임 파이프라인 엔진
// ─────────────────────────────────────────────

void print_words(const std::vector<std::string>& words, std::string_view label) {
    std::cout << "  [" << label << "] ";
    for (const auto& w : words) std::cout << '"' << w << "\" ";
    std::cout << '\n';
}

std::vector<std::string> apply_pipeline(
    const std::vector<std::string>& words,
    const std::vector<int>& ops)
{
    // op: 1=대문자, 2=소문자, 3=뒤집기, 4=모음 있는 단어만, 5=4글자 이상만, 6=정렬
    auto result = words;

    for (int op : ops) {
        switch (op) {
        case 1: {
            std::vector<std::string> tmp;
            for (auto w : result | std::views::transform(to_upper))
                tmp.push_back(std::move(w));
            result = std::move(tmp);
            break;
        }
        case 2: {
            std::vector<std::string> tmp;
            for (auto w : result | std::views::transform(to_lower))
                tmp.push_back(std::move(w));
            result = std::move(tmp);
            break;
        }
        case 3: {
            std::vector<std::string> tmp;
            for (auto w : result | std::views::transform(reverse_str))
                tmp.push_back(std::move(w));
            result = std::move(tmp);
            break;
        }
        case 4: {
            std::vector<std::string> tmp;
            for (auto w : result | std::views::filter(has_vowel))
                tmp.push_back(w);
            result = std::move(tmp);
            break;
        }
        case 5: {
            std::vector<std::string> tmp;
            for (auto w : result | std::views::filter([](const std::string& s){ return s.size() >= 4; }))
                tmp.push_back(w);
            result = std::move(tmp);
            break;
        }
        case 6:
            std::ranges::sort(result);
            break;
        default: break;
        }
    }
    return result;
}

// ─────────────────────────────────────────────
// 스테이지 정의
// ─────────────────────────────────────────────

struct Stage {
    std::string title;
    std::vector<std::string> input;
    std::vector<std::string> target;
    std::vector<int> hint;
    std::string description;
};

std::vector<Stage> make_stages() {
    return {
        {
            "스테이지 1: 대문자 변환 후 정렬",
            {"banana", "apple", "cherry", "date"},
            {"APPLE", "BANANA", "CHERRY", "DATE"},
            {1, 6},
            "모두 대문자로 변환 후 알파벳 순으로 정렬하세요."
        },
        {
            "스테이지 2: 모음 필터 + 뒤집기",
            {"sky", "ocean", "rain", "mph", "echo"},
            {"naeco", "niar", "ohce"},
            {4, 3},
            "모음이 있는 단어만 남긴 뒤 각 단어를 뒤집으세요."
        },
        {
            "스테이지 3: 긴 단어 + 소문자 + 정렬",
            {"Elephant", "Cat", "Rhinoceros", "Dog", "Butterfly"},
            {"butterfly", "elephant", "rhinoceros"},
            {5, 2, 6},
            "4글자 이상 단어만 남기고, 소문자로 변환 후 정렬하세요."
        },
    };
}

// ─────────────────────────────────────────────
// 자동 데모 모드 (사용자 입력 없이 진행)
// ─────────────────────────────────────────────

int main() {
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║  Ranges 파이프라인 텍스트 게임        ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";

    std::cout << "변환 연산자 목록:\n";
    std::cout << "  1 = 대문자(UPPER)  2 = 소문자(lower)\n";
    std::cout << "  3 = 뒤집기         4 = 모음 있는 단어만\n";
    std::cout << "  5 = 4글자 이상만   6 = 알파벳 정렬\n\n";

    auto stages = make_stages();
    int cleared = 0;

    for (auto& stage : stages) {
        std::cout << "─────────────────────────────────────\n";
        std::cout << stage.title << '\n';
        std::cout << "  설명: " << stage.description << '\n';
        print_words(stage.input, "입력");
        print_words(stage.target, "목표");

        // 데모: 힌트(정답)로 자동 시도
        std::cout << "  → 파이프라인 적용: ";
        for (int op : stage.hint) std::cout << op << ' ';
        std::cout << '\n';

        auto result = apply_pipeline(stage.input, stage.hint);
        print_words(result, "결과");

        bool ok = (result == stage.target);
        if (ok) {
            std::cout << "  ✅ 클리어!\n";
            ++cleared;
        } else {
            std::cout << "  ❌ 불일치 — 디버깅 필요\n";
        }
        std::cout << '\n';
    }

    std::cout << "═════════════════════════════════════\n";
    std::cout << "최종 결과: " << cleared << "/" << stages.size()
              << " 스테이지 클리어\n\n";

    // 보너스: 단어 빈도 분석 파이프라인
    std::cout << "=== 보너스: 단어 길이 분포 (ranges 파이프라인) ===\n";
    std::vector<std::string> corpus{
        "the","quick","brown","fox","jumps","over",
        "the","lazy","dog","a","fox","and","the","dog"
    };

    // 중복 제거 후 정렬
    auto dedup = corpus;
    std::ranges::sort(dedup);
    auto unique_end = std::ranges::unique(dedup);
    dedup.erase(unique_end.begin(), dedup.end());

    std::cout << "  고유 단어 (" << dedup.size() << "개): ";
    for (const auto& w : dedup) std::cout << w << ' ';
    std::cout << '\n';

    // 길이별 그룹
    std::map<std::size_t, std::vector<std::string>> by_len;
    for (const auto& w : dedup)
        by_len[w.size()].push_back(w);

    std::cout << "  길이별 그룹:\n";
    for (auto& [len, ws] : by_len) {
        std::cout << "    " << len << "글자: ";
        for (const auto& w : ws) std::cout << w << ' ';
        std::cout << '\n';
    }

    // fold_left로 총 문자 수
    auto total_chars = std::ranges::fold_left(
        dedup | std::views::transform([](const std::string& s){ return s.size(); }),
        std::size_t{0}, std::plus<std::size_t>{}
    );
    std::cout << "  총 문자 수(고유): " << total_chars << '\n';

    return 0;
}
