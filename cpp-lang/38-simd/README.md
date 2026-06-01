# C++ Session 38: SIMD & Compiler Intrinsics

## 학습 목표

- 단일 명령 다중 데이터 (SIMD) 패러다임 이해
- GCC/Clang 컴파일러 내장 함수 활용
- 비트 연산 최적화 기법 마스터
- 분기 예측과 캐시 친화적 코딩

## 핵심 개념

### 1. SIMD (Single Instruction Multiple Data)
- 하나의 명령으로 여러 데이터를 동시에 처리
- CPU 벡터 명령 (SSE, AVX, NEON 등)
- C++17에서는 직접 벡터 타입을 지원하지 않으므로, 수동 최적화와 컴파일러 내장 함수 사용

### 2. 컴파일러 내장 함수
- `__builtin_popcount()` — 설정된 비트 개수 계산 (bit counting)
- `__builtin_clz()` — 앞의 영 비트 개수 (leading zeros)
- `__builtin_ctz()` — 뒤의 영 비트 개수 (trailing zeros)
- `__builtin_parity()` — 비트 패리티

### 3. 비트 해킹 기법
- 비트 연산을 통한 고속 계산
- 메모리 효율성과 캐시 최적화
- 분기 제거를 통한 성능 개선

### 4. 루프 언롤링 (Loop Unrolling)
- 루프 오버헤드 감소
- 명령어 레벨 병렬화 향상
- 컴파일러 최적화 힌트

### 5. 캐시 친화적 알고리즘
- 데이터 지역성 (Data Locality)
- 메모리 접근 패턴 최적화
- False Sharing 회피

## 과제

### `homework.cpp` — 비트 연산 최적화
- 비트 조작 함수들 구현
- 컴파일러 내장 함수 활용하여 성능 측정
- 전통적인 방식 vs 내장 함수 비교

### `homework2.cpp` — 고급 비트 해킹
- 비트 집합 구현 (Bitset simulation)
- 해밍 무게 (Hamming Weight) 고속 계산
- 그레이 코드 변환, MSB/LSB 추출

### `game1.cpp` — 입자 시스템 성능 게임
- 입자 시스템 구현 (간단한 물리)
- 수동 SIMD 최적화 (루프 언롤링)
- 성능 개선 측정 및 게임화

## 컴파일

```bash
# 표준 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/38-simd/homework.cpp -o hw38

# 최적화 활성화 (권장 - 성능 측정용)
g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 cpp-lang/38-simd/homework.cpp -o hw38

# 엄격한 플래그
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion cpp-lang/38-simd/game1.cpp -o game38
```

## 실행 예

```bash
./hw38       # 비트 연산 기본 연습
./hw38_2     # 고급 비트 해킹
./game38     # 입자 시스템 게임
```

## 참고

- GCC/Clang 내장 함수: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
- Bit Hacks (Stanford): https://graphics.stanford.edu/~seander/bithacks.html
- SIMD 개념과 최적화 전략

## 핵심 학습 포인트

1. **컴파일러 내장 함수**는 수동 구현보다 빠르고 이식성이 우수
2. **비트 연산**은 저수준 최적화의 핵심
3. **루프 언롤링**과 **분기 예측** 활용으로 성능 향상
4. **메모리 지역성**을 고려한 알고리즘 설계
5. 실제 성능 측정이 최적화의 시작점
