# Session 35: 표현식 템플릿 (Expression Templates)

## 학습 목표
- 표현식 템플릿(Expression Templates) 개념 이해
- 임시 객체 생성 최소화를 통한 성능 최적화
- 지연 평가(Lazy Evaluation) 패턴
- 복합 연산의 효율적인 구현

## 핵심 개념

### 1. 표현식 템플릿의 필요성
```cpp
Vector a(1000), b(1000), c(1000);
Vector d = a + b + c;  // 중간 임시 객체 2개 생성
```

### 2. 표현식 템플릿 원리
- 연산 결과를 즉시 계산하지 않음
- 표현식의 구조를 템플릿으로 인코딩
- 필요할 때만 최종 계산 수행

### 3. 벡터 연산 예제
```cpp
struct Add { ... };
struct Mul { ... };
template <typename L, typename R>
class BinOp { ... };
```

## 주요 패턴

### 표현식 노드 (Expression Node)
- 리프 노드: 벡터, 스칼라 래퍼
- 내부 노드: Add, Mul 등 연산자

### 트리 구조 (Expression Tree)
```cpp
(a + b) * c  // BinOp<BinOp<Add>, Mul>
```

### 평가 (Evaluation)
```cpp
void assign(Vector& result, const BinOp& expr) {
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = expr[i];  // 인덱스별로 계산
    }
}
```

## 성능 이점
- 중간 벡터 할당 제거
- 캐시 효율성 향상
- 컴파일러 최적화 용이

## 파일 구성

| 파일 | 설명 |
|------|------|
| `homework.cpp` | 기본 벡터 연산과 표현식 템플릿 구현 |
| `homework2.cpp` | 행렬 연산 및 스칼라 연산 지원 |
| `game1.cpp` | 성능 비교 게임 (일반 vs 표현식 템플릿) |

---

## 컴파일 및 실행

```bash
# Session 35 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/35-expression-templates/homework.cpp -o hw35
./hw35

g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/35-expression-templates/homework2.cpp -o hw35_2
./hw35_2

g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/35-expression-templates/game1.cpp -o game35
./game35
```

## 핵심 배울 점
1. 컴파일 타임 메타프로그래밍으로 성능 최적화
2. 임시 객체 제거의 중요성
3. 과학 컴퓨팅 라이브러리(Eigen, Blaze) 원리 이해
4. 지연 평가와 표현식 구성의 장점
