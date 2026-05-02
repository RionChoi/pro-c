#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // 학습: std::sort, std::find, std::count 기초 사용

    // 벡터 선언 및 초기화
    std::vector<int> numbers = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};

    std::cout << "=== STL 알고리즘 기초 ===\n\n";

    // 원본 벡터 출력
    std::cout << "원본 벡터: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // std::sort 사용 — 오름차순 정렬
    std::vector<int> sorted_asc = numbers;
    std::sort(sorted_asc.begin(), sorted_asc.end());

    std::cout << "오름차순 정렬: ";
    for (int num : sorted_asc) {
        std::cout << num << " ";
    }
    std::cout << "\n";

    // std::sort 사용 — 내림차순 정렬 (람다 함수 사용)
    std::vector<int> sorted_desc = numbers;
    std::sort(sorted_desc.begin(), sorted_desc.end(), 
              [](int a, int b) { return a > b; });

    std::cout << "내림차순 정렬: ";
    for (int num : sorted_desc) {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // std::find 사용 — 특정 값 찾기
    int target = 45;
    auto it = std::find(numbers.begin(), numbers.end(), target);

    if (it != numbers.end()) {
        std::cout << "값 " << target << "를 찾았습니다!\n";
        std::cout << "인덱스: " << std::distance(numbers.begin(), it) << "\n";
    } else {
        std::cout << "값 " << target << "을 찾지 못했습니다.\n";
    }
    std::cout << "\n";

    // std::count 사용 — 특정 값의 개수 세기
    int value_to_count = 22;
    int count = std::count(numbers.begin(), numbers.end(), value_to_count);
    std::cout << "값 " << value_to_count << "의 개수: " << count << "\n";

    // std::count_if 사용 — 조건을 만족하는 원소 개수
    int count_greater_50 = std::count_if(numbers.begin(), numbers.end(),
                                         [](int num) { return num > 50; });
    std::cout << "50보다 큰 수의 개수: " << count_greater_50 << "\n";

    return 0;
}
