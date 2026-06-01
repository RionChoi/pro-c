# Session 38: SIMD & 컴파일러 인트린직 (SIMD & Compiler Intrinsics)

## 학습 목표
1. **SIMD (Single Instruction Multiple Data)** 개념 이해
2. **벡터 연산** 활용 및 성능 최적화
3. **컴파일러 인트린직(intrinsics)** 직접 사용
4. **OpenMP 병렬화** 기본 활용
5. 성능 측정 및 최적화 기법

## 핵심 개념

### SIMD란?
- 하나의 명령어로 여러 데이터를 동시에 처리
- MMX, SSE, AVX, AVX-512 등 다양한 확장 명령어 세트
- CPU의 병렬 처리 능력을 최대한 활용

### 벡터화(Vectorization)
- 루프 기반 배열 연산을 벡터 연산으로 변환
- 컴파일러 최적화 플래그 활용 (`-march=native`, `-O3`)
- 수동 벡터화 vs 자동 벡터화

### 컴파일러 인트린직
- `<immintrin.h>` 헤더로 SIMD 명령어 직접 호출
- `__m128`, `__m256`, `__m512` 등 SIMD 데이터 타입
- 명시적 최적화가 필요한 성능 핵심 부분에서만 사용

### OpenMP 병렬화
- `#pragma omp parallel for` 지시문
- 간단한 병렬화로 멀티코어 활용
- 개인 변수, 공유 변수 관리

## 파일 설명
- **homework.cpp**: 기본 SIMD 연산 (정수/실수 덧셈, 곱셈)
- **homework2.cpp**: 복합 벡터 연산 (내적, 행렬 곱셈의 한 행)
- **game1.cpp**: 성능 비교 게임 (순차 vs 벡터화 vs SIMD 성능 측정)

## 컴파일
```bash
# 기본 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw38

# 최적화 활성화 (SIMD 활용)
g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 -march=native homework.cpp -o hw38

# OpenMP 병렬화 포함
g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 -march=native -fopenmp game1.cpp -o game38
```

## 학습 포인트
- SIMD는 **데이터 병렬성**이 있는 연산에만 유효
- 컴파일러 최적화 플래그 `-O3 -march=native`의 중요성
- 메모리 정렬(alignment)이 성능에 미치는 영향
- 벡터화 루프는 데이터 의존성이 없어야 함
