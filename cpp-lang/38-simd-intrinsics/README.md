# Session 38: SIMD & 컴파일러 인트린직

## 학습 목표
- SIMD (Single Instruction Multiple Data) 기본 개념 이해
- SSE/AVX 명령어 활용
- 컴파일러 인트린직 함수 사용
- 벡터화된 계산으로 성능 최적화
- 이식성과 성능의 균형

## 핵심 개념

### SIMD 개요
- 하나의 명령어로 여러 데이터를 동시에 처리
- 부동소수점 배열, 정수 배열 연산 최적화
- 과학 계산, 게임 엔진, 이미지 처리에서 중요

### 컴파일러 인트린직
```cpp
#include <immintrin.h>  // SSE/AVX 인트린직
__m128 vec = _mm_set_ps(4.0f, 3.0f, 2.0f, 1.0f);  // 128비트 벡터
__m128 result = _mm_add_ps(vec, vec);              // 병렬 덧셈
```

### 성능 측정
- 시간 측정 (std::chrono)
- SIMD vs 스칼라 비교
- 이득(speedup) 계산

## 과제

### homework.cpp
벡터 내적(Dot Product) 계산:
- 스칼라 버전 작성
- SSE 버전 작성 (__m128 사용)
- 성능 비교

### homework2.cpp
행렬-벡터 곱셈 (Matrix-Vector Multiplication):
- 작은 4x4 행렬 × 4D 벡터
- 스칼라 루프 vs AVX 최적화
- 속도 개선율 측정

### game1.cpp
**3D 벡터 연산 시뮬레이션 (Particle Physics)**
- 입자 100개 생성 (위치, 속도, 가속도)
- 매 프레임 위치 업데이트 (SIMD 활용)
- 충돌 검사 (단순화)
- 성능 출력
- 게임 루프 시뮬레이션 (10 프레임)

```
입자 100개 생성...
[프레임 0] 처리 시간: 0.05ms
[프레임 1] 처리 시간: 0.04ms
...
[프레임 9] 처리 시간: 0.06ms
총 시간: 0.48ms (평균 0.048ms/frame)
최적화 이득: 약 2.8x (스칼라 대비)
```

## 컴파일
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 -mavx2 homework.cpp -o hw38
g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 -mavx2 game1.cpp -o game38
```

## 주의사항
- `-O3 -mavx2` 플래그 필수 (최적화 + AVX 지원)
- 이식성: 실행 환경에서 SSE/AVX 지원 여부 확인
- 컴파일러별로 인트린직 함수명이 약간 다를 수 있음
