#include <iostream>
#include <vector>
#include <limits>

int main(void) {
    std::cout << "=== 배열 & 벡터 과제 1 ===\n\n";

    int n;
    std::cout << "입력할 정수의 개수를 입력하세요: ";
    if (!(std::cin >> n)) {
        std::cerr << "입력 오류: 정수를 입력해주세요.\n";
        return 1;
    }
    if (n <= 0) {
        std::cerr << "입력 오류: 양수를 입력해주세요.\n";
        return 1;
    }

    std::vector<int> vec;
    vec.reserve(n);

    std::cout << n << "개의 정수를 입력하세요 (공백으로 구분): ";
    for (int i = 0; i < n; ++i) {
        int val;
        if (!(std::cin >> val)) {
            std::cerr << "입력 오류: 올바른 정수를 입력해주세요.\n";
            return 1;
        }
        vec.push_back(val);
    }

    // 합계 계산
    long long sum = 0;
    int max_val = vec[0];
    int min_val = vec[0];
    for (int val : vec) {
        sum += val;
        if (val > max_val) max_val = val;
        if (val < min_val) min_val = val;
    }
    double average = static_cast<double>(sum) / n;

    std::cout << "\n결과:\n";
    std::cout << "합계: " << sum << "\n";
    std::cout << "평균: " << average << "\n";
    std::cout << "최대값: " << max_val << "\n";
    std::cout << "최소값: " << min_val << "\n\n";

    // 검색
    std::cout << "검색할 값: ";
    int target;
    if (!(std::cin >> target)) {
        std::cerr << "입력 오류.\n";
        return 1;
    }

    bool found = false;
    int found_index = -1;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == target) {
            found = true;
            found_index = static_cast<int>(i);
            break;
        }
    }

    if (found) {
        std::cout << target << " 발견됨 (인덱스 " << found_index << ")\n";
    } else {
        std::cout << target << " 은(는) 존재하지 않습니다.\n";
    }

    return 0;
}
