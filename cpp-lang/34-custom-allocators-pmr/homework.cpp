#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstddef>
#include <memory_resource>

int main() {
    std::cout << "=== std::pmr 기본 사용법 ===\n\n";

    // 스택 기반 메모리 버퍼 (1MB)
    std::array<std::byte, 1024 * 1024> buffer;
    std::pmr::monotonic_buffer_resource mbr(buffer.data(), buffer.size());

    // pmr 벡터 생성 (메모리 리소스 지정)
    std::pmr::vector<int> vec(&mbr);
    vec.reserve(10);

    std::cout << "1. monotonic_buffer_resource 사용:\n";
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i * 100);
    }

    std::cout << "   벡터 크기: " << vec.size() << "\n";
    std::cout << "   벡터 내용: ";
    for (int v : vec) {
        std::cout << v << " ";
    }
    std::cout << "\n\n";

    // 문자열 벡터
    std::pmr::vector<std::pmr::string> strings(&mbr);

    std::cout << "2. pmr::string 벡터:\n";
    strings.push_back(std::pmr::string("Hello", &mbr));
    strings.push_back(std::pmr::string("World", &mbr));
    strings.push_back(std::pmr::string("PMR", &mbr));

    for (const auto& s : strings) {
        std::cout << "   " << s << "\n";
    }
    std::cout << "\n";

    // 기본 할당자와 비교
    std::cout << "3. 기본 할당자 vs pmr 할당자:\n";
    std::vector<int> normal_vec;
    normal_vec.push_back(42);
    std::cout << "   정상 벡터: " << normal_vec[0] << "\n";

    std::pmr::vector<int> pmr_vec(&mbr);
    pmr_vec.push_back(42);
    std::cout << "   PMR 벡터:  " << pmr_vec[0] << "\n\n";

    // 중첩된 컨테이너
    std::cout << "4. 중첩 컨테이너 (std::pmr 사용):\n";
    std::pmr::vector<std::pmr::vector<int>> matrix(&mbr);

    for (int row = 0; row < 3; ++row) {
        std::pmr::vector<int> row_data(&mbr);
        for (int col = 0; col < 4; ++col) {
            row_data.push_back((row + 1) * (col + 1));
        }
        matrix.push_back(row_data);
    }

    for (const auto& row : matrix) {
        std::cout << "   [";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << row[i];
            if (i < row.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout << "\n";

    std::cout << "학습 완료: std::pmr 기본 개념 이해\n";
    return 0;
}
