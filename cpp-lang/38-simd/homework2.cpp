#include <iostream>
#include <vector>
#include <cstdint>
#include <bitset>
#include <array>

using namespace std;

// 고급 비트 해킹 기법들

// 그레이 코드로 변환 (이진 ↔ 그레이)
uint32_t binary_to_gray(uint32_t n) {
    return n ^ (n >> 1);
}

// 그레이 코드에서 이진으로 변환
uint32_t gray_to_binary(uint32_t gray) {
    uint32_t binary = gray;
    for (uint32_t mask = gray >> 1; mask; mask >>= 1) {
        binary ^= mask;
    }
    return binary;
}

// 다음 2^k 구하기 (최소 2^k ≥ n)
uint32_t next_power_of_two(uint32_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

// 2의 거듭제곱인지 확인
bool is_power_of_two(uint32_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// 가장 큰 비트만 추출
uint32_t isolate_msb(uint32_t n) {
    if (n == 0) return 0;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n - (n >> 1);
}

// 가장 작은 비트만 추출
uint32_t isolate_lsb(uint32_t n) {
    return n & -static_cast<int32_t>(n);
}

// i번째 비트 찾기 (0부터 시작, LSB부터)
uint32_t find_kth_bit(uint32_t n, int k) {
    while (k--) {
        n &= n - 1;  // 가장 낮은 비트 제거
    }
    return n & -static_cast<int32_t>(n);
}

// 비트 필드 추출
uint32_t extract_bits(uint32_t val, int start, int len) {
    return (val >> start) & ((1U << len) - 1);
}

// 비트 필드 설정
uint32_t set_bits(uint32_t val, int start, int len, uint32_t new_bits) {
    uint32_t mask = ((1U << len) - 1) << start;
    return (val & ~mask) | ((new_bits << start) & mask);
}

// 비트 회전 (왼쪽)
uint32_t rotate_left(uint32_t n, int bits) {
    bits &= 31;
    return (n << bits) | (n >> (32 - bits));
}

// 비트 회전 (오른쪽)
uint32_t rotate_right(uint32_t n, int bits) {
    bits &= 31;
    return (n >> bits) | (n << (32 - bits));
}

// 비트 스왑 (i번째와 j번째 비트)
uint32_t swap_bits(uint32_t n, int i, int j) {
    uint32_t bit_i = (n >> i) & 1;
    uint32_t bit_j = (n >> j) & 1;
    if (bit_i != bit_j) {
        n ^= (1U << i) | (1U << j);
    }
    return n;
}

// XOR로 합 구하기 (캐리 없음)
uint32_t add_without_carry(uint32_t a, uint32_t b) {
    while (b != 0) {
        uint32_t carry = (a & b) << 1;
        a ^= b;
        b = carry;
    }
    return a;
}

// 간단한 비트 벡터 (고정 크기 bitset)
class BitVector {
private:
    static constexpr int BITS_PER_WORD = 32;
    vector<uint32_t> data;
    int num_bits;

public:
    explicit BitVector(int n) : num_bits(n) {
        data.resize((n + BITS_PER_WORD - 1) / BITS_PER_WORD, 0);
    }

    void set(int pos) {
        if (pos < num_bits) {
            data[pos / BITS_PER_WORD] |= (1U << (pos % BITS_PER_WORD));
        }
    }

    void clear(int pos) {
        if (pos < num_bits) {
            data[pos / BITS_PER_WORD] &= ~(1U << (pos % BITS_PER_WORD));
        }
    }

    bool test(int pos) const {
        if (pos < num_bits) {
            return (data[pos / BITS_PER_WORD] >> (pos % BITS_PER_WORD)) & 1;
        }
        return false;
    }

    int count() const {
        int total = 0;
        for (auto word : data) {
            total += __builtin_popcount(word);
        }
        return total;
    }

    int size() const { return num_bits; }
};

int main() {
    cout << "=== C++ 고급 비트 해킹 기법 ===\n\n";

    // 1. 그레이 코드
    cout << "1. 그레이 코드 변환\n";
    cout << "---\n";
    for (int i = 0; i < 8; i++) {
        uint32_t gray = binary_to_gray(i);
        uint32_t back = gray_to_binary(gray);
        cout << "이진: " << i << " → 그레이: " << gray << " → 이진: " << back << "\n";
    }
    cout << "\n";

    // 2. 2의 거듭제곱 연산
    cout << "2. 2의 거듭제곱 연산\n";
    cout << "---\n";
    vector<uint32_t> test_vals = {3, 5, 16, 17, 100, 1000};
    for (auto val : test_vals) {
        cout << val << " → ";
        cout << "다음 2^k: " << next_power_of_two(val) << " ";
        cout << "2의 거듭제곱? " << (is_power_of_two(val) ? "예" : "아니오") << " ";
        cout << "MSB: 0x" << hex << isolate_msb(val) << dec << "\n";
    }
    cout << "\n";

    // 3. 비트 추출/설정
    cout << "3. 비트 필드 조작\n";
    cout << "---\n";
    uint32_t val = 0xABCDEF00;
    cout << "원본: 0x" << hex << val << dec << "\n";
    cout << "비트 [8:15] 추출: 0x" << hex << extract_bits(val, 8, 8) << dec << "\n";
    uint32_t modified = set_bits(val, 8, 8, 0xFF);
    cout << "비트 [8:15] = 0xFF 설정: 0x" << hex << modified << dec << "\n\n";

    // 4. 비트 회전
    cout << "4. 비트 회전\n";
    cout << "---\n";
    uint32_t rot_val = 0x12345678;
    cout << "원본: 0x" << hex << rot_val << dec << "\n";
    cout << "왼쪽 8칸: 0x" << hex << rotate_left(rot_val, 8) << dec << "\n";
    cout << "오른쪽 4칸: 0x" << hex << rotate_right(rot_val, 4) << dec << "\n\n";

    // 5. XOR 덧셈 (캐리 없음)
    cout << "5. XOR 덧셈 (캐리 없음)\n";
    cout << "---\n";
    for (int a : {1, 3, 7, 15}) {
        for (int b : {1, 2, 4}) {
            cout << a << " ⊕+ " << b << " = " << add_without_carry(a, b);
            cout << " (일반 덧셈: " << (a + b) << ")\n";
        }
    }
    cout << "\n";

    // 6. 비트 벡터
    cout << "6. 비트 벡터 사용\n";
    cout << "---\n";
    BitVector bv(32);
    for (int i = 0; i < 32; i += 3) {
        bv.set(i);
    }
    cout << "설정된 비트 개수: " << bv.count() << "\n";
    cout << "비트 벡터 상태: ";
    for (int i = 0; i < 32; i++) {
        cout << (bv.test(i) ? "1" : "0");
        if ((i + 1) % 8 == 0) cout << " ";
    }
    cout << "\n\n";

    // 7. 비트 스왑
    cout << "7. 비트 스왑 (홀짝 비트)\n";
    cout << "---\n";
    uint32_t swap_val = 0b10101010;
    cout << "원본:     0b" << bitset<8>(swap_val) << "\n";
    swap_val = swap_bits(swap_val, 0, 1);
    swap_val = swap_bits(swap_val, 2, 3);
    swap_val = swap_bits(swap_val, 4, 5);
    swap_val = swap_bits(swap_val, 6, 7);
    cout << "스왑 후:  0b" << bitset<8>(swap_val) << "\n\n";

    cout << "[참고] 이 기법들은 고성능 컴퓨팅과 저수준 최적화에 사용됩니다.\n";

    return 0;
}
