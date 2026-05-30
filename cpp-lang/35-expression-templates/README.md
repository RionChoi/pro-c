# Session 35: 표현식 템플릿 (Expression Templates)

## 학습 목표
- 표현식 템플릿의 개념과 원리 이해
- CRTP와 표현식 템플릿의 결합
- 지연 평가(lazy evaluation) 패턴
- 벡터 연산 최적화
- 메타프로그래밍을 통한 성능 향상

## 핵심 개념

### 1. 표현식 템플릿의 목적
- 중간 벡터 생성 방지
- 메모리 할당 최소화
- 컴파일 타임 최적화

### 2. 기본 구조
```cpp
template <typename Lhs, typename Rhs>
class VectorAdd {
    const Lhs& lhs;
    const Rhs& rhs;
    
public:
    VectorAdd(const Lhs& l, const Rhs& r) : lhs(l), rhs(r) {}
    auto operator[](std::size_t i) const {
        return lhs[i] + rhs[i];
    }
};
```

### 3. CRTP 활용
- Curiously Recurring Template Pattern
- 인덱싱 인터페이스 통일
- 암묵적 변환(implicit conversion) 지원

### 4. 성능 최적화
- 임시 객체 생성 제거
- 컴파일러 루프 언롤
- SIMD 활용 가능

## 참고 자료
- Boost.Proto (Expression Templates 라이브러리)
- Blitz++ (수치 계산용 벡터 라이브러리)
