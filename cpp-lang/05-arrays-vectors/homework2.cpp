#include <iostream>
#include <vector>
#include <algorithm>

int main(void) {
    std::cout << "=== 배열 & 벡터 과제 2 ===\n\n";

    // --- 벡터 1 입력 ---
    std::cout << "--- 벡터 1 ---\n";
    int n1;
    std::cout << "벡터 1의 요소 개수: ";
    if (!(std::cin >> n1) || n1 <= 0) {
        std::cerr << "입력 오류: 양수를 입력해주세요.\n";
        return 1;
    }

    std::vector<int> vec1;
    vec1.reserve(n1);
    std::cout << "벡터 1의 " << n1 << "개 정수를 입력: ";
    for (int i = 0; i < n1; ++i) {
        int v;
        if (!(std::cin >> v)) {
            std::cerr << "입력 오류.\n";
            return 1;
        }
        vec1.push_back(v);
    }

    // --- 벡터 2 입력 ---
    std::cout << "\n--- 벡터 2 ---\n";
    int n2;
    std::cout << "벡터 2의 요소 개수: ";
    if (!(std::cin >> n2) || n2 <= 0) {
        std::cerr << "입력 오류: 양수를 입력해주세요.\n";
        return 1;
    }

    std::vector<int> vec2;
    vec2.reserve(n2);
    std::cout << "벡터 2의 " << n2 << "개 정수를 입력: ";
    for (int i = 0; i < n2; ++i) {
        int v;
        if (!(std::cin >> v)) {
            std::cerr << "입력 오류.\n";
            return 1;
        }
        vec2.push_back(v);
    }

    // --- 합계 & 평균 ---
    long long sum1 = 0, sum2 = 0;
    for (int v : vec1) sum1 += v;
    for (int v : vec2) sum2 += v;

    double avg1 = static_cast<double>(sum1) / n1;
    double avg2 = static_cast<double>(sum2) / n2;

    std::cout << "\n벡터 1 — 합계: " << sum1 << ", 평균: " << avg1 << "\n";
    std::cout << "벡터 2 — 합계: " << sum2 << ", 평균: " << avg2 << "\n";

    // --- 더 큰 평균 ---
    if (avg1 > avg2) {
        std::cout << "더 큰 평균을 가진 벡터: 벡터 1 (" << avg1 << ")\n";
    } else if (avg2 > avg1) {
        std::cout << "더 큰 평균을 가진 벡터: 벡터 2 (" << avg2 << ")\n";
    } else {
        std::cout << "두 벡터의 평균이 같습니다.\n";
    }

    // --- 공통 요소 ---
    std::cout << "\n공통 요소: ";
    std::vector<int> common;
    for (int v : vec1) {
        if (std::find(vec2.begin(), vec2.end(), v) != vec2.end() &&
            std::find(common.begin(), common.end(), v) == common.end()) {
            common.push_back(v);
        }
    }

    if (common.empty()) {
        std::cout << "없음\n";
    } else {
        for (size_t i = 0; i < common.size(); ++i) {
            std::cout << common[i];
            if (i < common.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }

    return 0;
}
