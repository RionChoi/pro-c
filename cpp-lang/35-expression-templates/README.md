# C++ Session 35: 표현식 템플릿 (Expression Templates)

## 학습 목표
- 표현식 템플릿(Expression Template, ET)의 개념 이해
- 나이브 벡터 연산의 성능 문제 파악
- 표현식 템플릿을 통한 성능 최적화
- 컴파일 타임 계산 및 레이지 평가(lazy evaluation)
- 실제 수치 계산 라이브러리의 설계 원리

## 핵심 개념

### 1. 문제점: 임시 객체 생성
```cpp
Vector a, b, c;
Vector result = a + b + c;
// 일반 오버로딩: 중간에 3개의 임시 벡터 생성 → 메모리 할당/복사 반복
```

### 2. 표현식 템플릿의 아이디어
- 연산을 즉시 수행하지 않음
- 연산의 구조를 컴파일 타임에 기록
- 최종 할당 시에만 한 번에 계산 (fusion)
- 중간 임시 객체 완전 제거

### 3. 핵심 구현
- **Expression node**: 연산 구조 표현 (e.g., `BinaryOp<Op, L, R>`)
- **Lazy evaluation**: 평가 미연기
- **Operator overloading**: 표현식 생성
- **Array fusion**: 반복문 한 번에 모든 연산 처리

### 4. 장점
- 캐시 효율성 증대
- 메모리 할당 최소화
- SIMD 최적화 기회 증가
- 수치 계산 라이브러리(Eigen, Blitz++) 기반 기술

## 과제 설명

### homework.cpp
나이브 벡터 클래스와 표현식 템플릿 벡터 비교:
- SimpleVector: 일반적인 벡터 (각 연산마다 임시 객체)
- ExprVector: 표현식 템플릿 벡터 (임시 객체 제거)
- 성능 비교

### homework2.cpp
복잡한 수식의 표현식 템플릿 최적화:
- 스칼라 곱셈, 벡터 덧셈 혼합
- 유연한 표현식 지원
- 복합 연산의 효율성 검증

### game1.cpp
행렬 연산 게임 — 최고 성능 행렬 계산:
- 행렬 곱셈 최적화
- 다양한 행렬 크기 처리
- 계산 시간 측정 및 최적화 검증

## 컴파일 방법

```bash
# C++17 이상 필요
g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic homework.cpp -o hw35a
./hw35a

g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic homework2.cpp -o hw35b
./hw35b

g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic game1.cpp -o hw35c
./hw35c
```

## 주의
- **-O3 플래그 권장**: 인라인과 루프 최적화가 필수
- 디버그 모드에서는 성능 차이가 안 보일 수 있음
- Expression Template은 컴파일 타임 트레이드오프: 컴파일 시간 증가

## 참조
- Alexandrescu, "Modern C++ Design" (Chapter 11: Expression Templates)
- Eigen 라이브러리 구현
- Blitz++ 라이브러리
