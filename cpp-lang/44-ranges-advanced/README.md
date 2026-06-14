# 세션 44: C++20 Ranges 심화

## 학습 목표
1. 커스텀 `view` 타입을 `view_interface`로 직접 구현한다
2. range adaptor closure object를 만들어 `|` 파이프라인에 연결한다
3. `std::ranges` 알고리즘과 커스텀 view를 조합한 lazy evaluation 파이프라인을 설계한다
4. `std::views::transform`, `filter`, `take`, `drop`, `zip`, `enumerate` 등 표준 adaptor를 심화 활용한다
5. 무한 range(infinite range)와 `iota` 기반 생성기를 파이프라인에 활용한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| `std::ranges::view` | 경량(O(1) 복사), lazy evaluation, non-owning range |
| `view_interface<D>` | CRTP로 커스텀 view에 `begin/end/size/empty` 자동 제공 |
| range adaptor | `|` 연산자로 체이닝 가능한 변환 객체 |
| range adaptor closure | `range_adaptor_closure<T>` 상속으로 `|` 지원 구현 |
| lazy evaluation | 원소가 실제로 필요할 때만 계산됨 |
| `std::views::iota` | 무한 정수 수열 생성 |
| `std::views::zip` | 여러 range를 tuple로 묶음 (C++23에서 표준화) |
| `std::views::enumerate` | (index, value) 쌍으로 변환 (C++23) |
| projection | 알고리즘에 전달하는 멤버 접근 변환 함수 |

## 핵심 문법

```cpp
// 커스텀 view
struct MyView : std::ranges::view_interface<MyView> {
    auto begin() const { return ...; }
    auto end()   const { return ...; }
};

// range adaptor closure (C++23)
struct MyAdaptor : std::ranges::range_adaptor_closure<MyAdaptor> {
    template <std::ranges::viewable_range R>
    auto operator()(R&& r) const { return MyView{std::forward<R>(r)}; }
};
inline constexpr MyAdaptor my_adaptor{};

// 파이프라인
auto result = data | std::views::filter(pred) | my_adaptor | std::views::take(5);
```

## 컴파일 명령어

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
