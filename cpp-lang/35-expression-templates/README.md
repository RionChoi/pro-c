# Session 35: 표현식 템플릿 (Expression Templates)

## 학습 목표
- 표현식 템플릿(Expression Templates) 개념 이해
- 지연 평가(Lazy Evaluation)를 통한 성능 최적화
- 템플릿 메타프로그래밍(Template Metaprogramming)을 이용한 고급 최적화
- 복잡한 수식 계산의 메모리 효율성 향상

## 핵심 개념

### 1. 표현식 템플릿의 필요성
일반적인 벡터 연산:
```cpp
// 비효율적: 중간 결과 저장
Vector a(n), b(n), c(n);
Vector temp1 = a + b;
Vector temp2 = temp1 * 2;
Vector result = temp2 - c;  // 여러 임시 벡터 생성
```

표현식 템플릿 사용:
```cpp
// 효율적: 한 번에 계산
Vector result = a + b * 2 - c;  // 임시 벡터 없음
```

### 2. 핵심 구성 요소
- **Expression Node**: 연산을 나타내는 템플릿 클래스
- **Lazy Evaluation**: 필요할 때까지 계산 지연
- **CRTP Pattern**: 컴파일 타임 다형성

### 3. 기본 구조
```cpp
template<typename E>
class VecExpr {
public:
    double operator[](std::size_t i) const {
        return static_cast<const E&>(*this)[i];
    }
};

template<typename L, typename R>
class VecAdd : public VecExpr<VecAdd<L, R>> {
private:
    const L& l_;
    const R& r_;
public:
    VecAdd(const L& l, const R& r) : l_(l), r_(r) {}
    double operator[](std::size_t i) const {
        return l_[i] + r_[i];
    }
};
```

## 파일 구성
- `homework.cpp` - 기본 표현식 템플릿 구현
- `homework2.cpp` - 고급 최적화: 복합 연산과 성능 비교
- `game1.cpp` - 수식 계산 게임

## 핵심 주제
- 템플릿 메타프로그래밍
- 지연 평가 패턴
- 메모리 최적화
- 컴파일 타임 계산 그래프
- 성능 측정 및 벤치마크
