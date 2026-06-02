# Session 38: SIMD & Compiler Intrinsics

## 학습 목표
- SIMD (Single Instruction Multiple Data) 개념 이해
- `<immintrin.h>` 를 이용한 SSE/AVX 인트린직 활용
- 벡터 연산의 성능 최적화
- 메모리 정렬(alignment) 의 중요성

## 핵심 개념

### SIMD란?
단일 명령으로 여러 데이터를 병렬 처리하는 기술.

### SSE/AVX 인트린직
- `_mm_add_ps()` — 128비트 부동소수점 4개 더하기
- `_mm256_add_ps()` — 256비트 부동소수점 8개 더하기
- `_mm_load_ps()` / `_mm256_load_ps()` — 정렬된 메모리에서 로드
- `_mm_store_ps()` / `_mm256_store_ps()` — 정렬된 메모리에 저장

### 메모리 정렬
SIMD 명령은 정렬된 메모리 접근이 필수적이다:
- SSE: 16바이트 정렬
- AVX: 32바이트 정렬

## 과제 설명

1. **homework.cpp** — 벡터 내적 계산 (SIMD vs 일반)
2. **homework2.cpp** — 행렬 곱셈 (AVX 최적화)
3. **game1.cpp** — 이미지 처리 필터 게임

---
