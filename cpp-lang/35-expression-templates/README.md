# Session 35: 표현식 템플릿 (Expression Templates)

## 학습 목표
- 표현식 템플릿의 개념과 이점 이해
- Lazy evaluation 구현
- 벡터 연산 최적화
- 임시 객체 생성 최소화

## 핵심 개념
1. **표현식 템플릿(Expression Template)** — 계산을 늦추는 기법
2. **Lazy Evaluation** — 필요할 때까지 계산 미룸
3. **CRTP(Curiously Recurring Template Pattern)** 활용
4. **연산 오버로딩 최적화**
5. **벡터 수학 가속화**

## 실습 항목
- homework.cpp: 기본 표현식 템플릿 구현
- homework2.cpp: 벡터 연산 최적화
- game1.cpp: 수학 연산 게임 (행렬 계산)

## 컴파일 명령
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw35
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw35_2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game35
```

## 성능 이점
- 임시 객체 제거
- 연산 융합(Operation Fusion)
- 컴파일 타임 최적화
- SIMD 명령어 활용 가능
