#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>

void print_separator(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    // 1. 기본 할당자 (heap)
    print_separator("기본 할당자 테스트");
    {
        std::pmr::vector<int> vec;  // std::pmr::new_delete_resource() 사용
        for (int i = 0; i < 5; i++) {
            vec.push_back(i * 10);
        }
        std::cout << "벡터: ";
        for (int v : vec) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }

    // 2. Monotonic Buffer Resource (스택 기반)
    print_separator("Monotonic Buffer Resource");
    {
        char buffer[256];  // 스택 메모리
        std::pmr::monotonic_buffer_resource res(buffer, sizeof(buffer));

        std::pmr::vector<int> vec(&res);
        for (int i = 0; i < 10; i++) {
            vec.push_back(i * 5);
        }
        std::cout << "벡터 크기: " << vec.size() << '\n';
        std::cout << "벡터 데이터: ";
        for (int v : vec) {
            std::cout << v << " ";
        }
        std::cout << "\n";

        std::pmr::string str(&res);
        str = "monotonic 메모리 풀 테스트";
        std::cout << "문자열: " << str << '\n';
    }

    // 3. Unsynchronized Pool Resource (풀 기반)
    print_separator("Unsynchronized Pool Resource");
    {
        std::pmr::unsynchronized_pool_resource res;

        std::pmr::vector<int> ints(&res);
        for (int i = 0; i < 50; i++) {
            ints.push_back(i);
        }

        std::cout << "정수 벡터 크기: " << ints.size() << '\n';
        std::cout << "처음 10개: ";
        for (int i = 0; i < 10; i++) {
            std::cout << ints[i] << " ";
        }
        std::cout << "\n";
    }

    // 4. 메모리 효율성 비교
    print_separator("메모리 사용 패턴 비교");
    {
        // 동적 할당 버전
        std::cout << "동적 할당 (매번 new/delete):\n";
        {
            std::vector<int> vec;
            for (int i = 0; i < 100; i++) {
                vec.push_back(i);
            }
        }

        // Monotonic 버전 (한 번에 할당, 한 번에 해제)
        std::cout << "Monotonic (선형 할당):\n";
        {
            char buffer[2048];
            std::pmr::monotonic_buffer_resource res(buffer, sizeof(buffer));
            std::pmr::vector<int> vec(&res);
            for (int i = 0; i < 100; i++) {
                vec.push_back(i);
            }
        }

        std::cout << "메모리 할당 완료\n";
    }

    // 5. 중첩된 컨테이너
    print_separator("중첩된 컨테이너");
    {
        char buffer[1024];
        std::pmr::monotonic_buffer_resource res(buffer, sizeof(buffer));

        std::pmr::vector<std::pmr::vector<int>> matrix(&res);

        for (int i = 0; i < 3; i++) {
            std::pmr::vector<int> row(&res);
            for (int j = 0; j < 4; j++) {
                row.push_back(i * 4 + j);
            }
            matrix.push_back(row);
        }

        std::cout << "3x4 행렬:\n";
        for (const auto& row : matrix) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << "\n";
        }
    }

    return 0;
}
