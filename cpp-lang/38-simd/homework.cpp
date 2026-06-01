#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <iomanip>

using namespace std;

// 비트 연산 기본 함수들

// 설정된 비트 개수 세기 (전통 방식)
int popcount_naive(uint32_t n) {
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

// 설정된 비트 개수 세기 (Brian Kernighan 알고리즘)
int popcount_kernighan(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}

// 설정된 비트 개수 세기 (컴파일러 내장 함수)
int popcount_builtin(uint32_t n) {
    return __builtin_popcount(n);
}

// 앞의 영 비트 개수
int count_leading_zeros(uint32_t n) {
    if (n == 0) return 32;
    return __builtin_clz(n);
}

// 뒤의 영 비트 개수
int count_trailing_zeros(uint32_t n) {
    if (n == 0) return 32;
    return __builtin_ctz(n);
}

// 비트 패리티 (1의 개수가 홀수면 1, 짝수면 0)
int bit_parity(uint32_t n) {
    return __builtin_parity(n);
}

// 가장 큰 2의 거듭제곱 찾기 (MSB 위치)
int msb_position(uint32_t n) {
    if (n == 0) return -1;
    return 31 - __builtin_clz(n);
}

// 가장 작은 2의 거듭제곱 찾기 (LSB 위치)
int lsb_position(uint32_t n) {
    if (n == 0) return -1;
    return __builtin_ctz(n);
}

// 비트 반전
uint32_t reverse_bits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}

// 해밍 거리 계산 (두 수의 다른 비트 개수)
int hamming_distance(uint32_t x, uint32_t y) {
    return __builtin_popcount(x ^ y);
}

void print_bits(uint32_t n, int width = 8) {
    for (int i = width - 1; i >= 0; i--) {
        cout << ((n >> i) & 1);
    }
}

int main() {
    cout << "=== C++ SIMD & 컴파일러 내장 함수 ===\n\n";

    // 테스트 값
    vector<uint32_t> test_values = {0, 1, 15, 127, 255, 1024, 65535, 0x12345678};

    // 1. 비트 개수 세기 (Popcount)
    cout << "1. 설정된 비트 개수 (Popcount)\n";
    cout << "---\n";
    for (auto val : test_values) {
        cout << "0x" << hex << setw(8) << setfill('0') << val << dec << " → ";
        cout << "naive:" << popcount_naive(val) << " ";
        cout << "kernighan:" << popcount_kernighan(val) << " ";
        cout << "builtin:" << popcount_builtin(val) << "\n";
    }
    cout << "\n";

    // 2. 선행/후행 영 비트
    cout << "2. 선행/후행 영 비트\n";
    cout << "---\n";
    vector<uint32_t> nonzero_vals = {1, 8, 16, 256, 0x80000000};
    for (auto val : nonzero_vals) {
        cout << "0x" << hex << setw(8) << setfill('0') << val << dec << " → ";
        cout << "CLZ:" << count_leading_zeros(val) << " ";
        cout << "CTZ:" << count_trailing_zeros(val) << " ";
        cout << "MSB:" << msb_position(val) << " ";
        cout << "LSB:" << lsb_position(val) << "\n";
    }
    cout << "\n";

    // 3. 비트 패리티
    cout << "3. 비트 패리티 (1의 개수가 홀수면 1)\n";
    cout << "---\n";
    for (auto val : test_values) {
        cout << "0x" << hex << setw(8) << setfill('0') << val << dec << " → ";
        cout << "parity:" << bit_parity(val) << "\n";
    }
    cout << "\n";

    // 4. 해밍 거리
    cout << "4. 해밍 거리 (다른 비트 개수)\n";
    cout << "---\n";
    vector<pair<uint32_t, uint32_t>> pairs = {
        {1, 2}, {7, 4}, {0xFF, 0xF0}, {0x12345678, 0x87654321}
    };
    for (auto [x, y] : pairs) {
        cout << "0x" << hex << setw(8) << setfill('0') << x << " ↔ ";
        cout << setw(8) << setfill('0') << y << dec << " → ";
        cout << hamming_distance(x, y) << "\n";
    }
    cout << "\n";

    // 5. 비트 반전
    cout << "5. 비트 반전\n";
    cout << "---\n";
    for (auto val : {0xF0, 0x12, 0xAA}) {
        cout << "0x" << hex << setw(2) << setfill('0') << val << dec << " → ";
        cout << "0x" << hex << setw(2) << setfill('0') << (reverse_bits(val) >> 24) << dec << "\n";
    }
    cout << "\n";

    // 성능 비교: popcount 메서드들
    cout << "성능 비교 (1000만 번 반복)\n";
    cout << "---\n";
    constexpr int iterations = 10'000'000;

    auto benchmark = [iterations](const string& name, auto func) {
        auto start = chrono::high_resolution_clock::now();
        uint32_t sum = 0;
        for (int i = 0; i < iterations; i++) {
            sum += func(0x12345678 + i);
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << name << ": " << duration.count() << "ms (sum=" << sum << ")\n";
    };

    benchmark("Naive       ", popcount_naive);
    benchmark("Kernighan   ", popcount_kernighan);
    benchmark("Builtin CLZ ", [](uint32_t n) { return __builtin_popcount(n); });

    cout << "\n[참고] 컴파일러 내장 함수가 일반적으로 가장 빠릅니다.\n";
    cout << "GCC -O3 최적화 플래그로 컴파일하면 추가 성능 향상을 기대할 수 있습니다.\n";

    return 0;
}
