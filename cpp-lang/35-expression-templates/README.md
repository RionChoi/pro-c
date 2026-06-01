# Session 35: Expression Templates (표현식 템플릿)

## 학습 목표
- **Expression Templates (ET)** 패턴을 이해하고 구현한다.
- 컴파일 타임에 표현식을 AST(추상 구문 트리)로 구성하는 기법을 배운다.
- 임시 객체 제거를 통한 성능 최적화를 달성한다.
- Lazy evaluation을 활용한 지연 계산 기법을 습득한다.

## 핵심 개념

### 1. Expression Templates란?
- 연산식을 타입으로 표현하여 컴파일 타임에 최적화
- 벡터/행렬 연산에서 임시 객체 생성 제거
- Lazy evaluation으로 필요한 시점에만 계산 수행

### 2. ET의 이점
```cpp
// 일반적인 벡터 연산 (임시 객체 3개 생성)
v3 = v1 + v2 + v3;  // (v1 + v2) 임시 생성 → 전체 임시 생성

// Expression Template (임시 객체 0개)
v3 = v1 + v2 + v3;  // 표현식 타입만 생성, 최종 할당 시 계산
```

### 3. 구현 전략
- **Vector 클래스**: 기본 벡터 저장소
- **Expression 타입**: 연산 표현 (Add, Multiply, Scalar 등)
- **Binary Operation 템플릿**: 이진 연산 표현
- **Type Promotion**: 연산 결과 타입 결정
- **Subscript Operator**: 지연 평가 시점에 계산

## 과제 설명

### homework.cpp
- **Vector 클래스** 구현 (기본 벡터)
- **Expression 타입** 구현 (Add, Multiply)
- **Binary operator** 템플릿 정의
- 표현식 기반 연산 데모

### homework2.cpp
- **ScalarMultiplication** 구현 (스칼라 곱셈)
- **Type 특성 (Traits)** 구현
- **복합 표현식** (v1 + v2 * scalar)
- 성능 비교: 표현식 vs 임시 객체

### game1.cpp
- **Vector 계산 게임** 
- 사용자 입력으로 벡터 생성
- 표현식으로 선형 조합 (a*v1 + b*v2 + c*v3) 계산
- 결과 벡터 출력 및 거리/크기 계산

## 컴파일 및 실행

```bash
# homework.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework.cpp -o hw1
./hw1

# homework2.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2
./hw2

# game1.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion game1.cpp -o game1
./game1
```

## 주요 기법

### 1. Binary Expression Template
```cpp
template<typename Left, typename Right, typename Op>
class BinaryExpr {
    const Left& left;
    const Right& right;
public:
    double operator[](size_t i) const {
        return Op()(left[i], right[i]);
    }
};
```

### 2. Operator Overloading for Expression Building
```cpp
template<typename L, typename R>
auto operator+(const L& l, const R& r) {
    return BinaryExpr<L, R, Add>(l, r);
}
```

### 3. Lazy Evaluation
```cpp
auto expr = v1 + v2 + v3;  // 계산 안 함, 타입만 구성
double val = expr[0];      // 이 시점에 계산됨
```

## 참고
- Expression Templates는 C++ 성능 최적화의 고급 기법
- Boost.uBLAS, Eigen 같은 라이브러리에서 광범위하게 사용
- 컴파일 타임 오버헤드가 있지만 런타임 성능 향상이 더 큼
