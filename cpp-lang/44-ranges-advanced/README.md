# 세션 44: C++20 Ranges 심화

## 학습 목표
1. `std::ranges` 알고리즘과 `std::views` 파이프라인을 조합한다
2. lazy evaluation이 언제 실행되는지 관찰한다
3. 커스텀 View를 직접 구현해 range 인터페이스를 확장한다
4. range adaptor 함수로 재사용 가능한 파이프라인 조각을 만든다
5. 불필요한 임시 컨테이너 생성을 줄이는 파이프라인 최적화 습관을 익힌다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| range | `begin()`/`end()`로 순회 가능한 대상 |
| view | 원본 데이터를 소유하지 않거나 가볍게 소유하는 lazy range |
| adaptor | `filter`, `transform`, `take`처럼 view를 만드는 함수 객체 |
| lazy evaluation | 실제 순회 전까지 계산을 미루는 방식 |
| projection | 비교/검색 전에 요소에서 필요한 값만 꺼내는 함수 |
| view_interface | 커스텀 view 구현을 돕는 표준 기반 클래스 |

## 핵심 문법

```cpp
auto pipeline = values
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * n; })
    | std::views::take(5);
```

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
