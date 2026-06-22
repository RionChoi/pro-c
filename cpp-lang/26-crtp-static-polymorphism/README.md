# 세션 26: CRTP & 정적 다형성 (Curiously Recurring Template Pattern)

## 학습 목표

- CRTP(Curiously Recurring Template Pattern)의 구조와 동작 원리를 이해한다.
- 가상 함수(동적 다형성) 대신 CRTP(정적 다형성)를 활용하여 런타임 오버헤드를 제거한다.
- CRTP Mixin으로 공통 기능(비교 연산자, 클론, 싱글톤)을 재사용한다.
- 정책 기반 설계(Policy-based Design)를 CRTP로 구현한다.
- `static_cast<Derived*>(this)` 관용구를 안전하게 사용한다.

---

## 핵심 개념

### 1. CRTP 기본 구조

```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        // 컴파일 타임에 Derived::implementation() 호출 결정
        static_cast<Derived*>(this)->implementation();
    }
};

class Concrete : public Base<Concrete> {
public:
    void implementation() { std::cout << "구체 구현\n"; }
};
```

파생 클래스가 기반 클래스의 **템플릿 인자**로 자기 자신을 전달한다.
`static_cast<Derived*>(this)`로 파생 클래스 메서드를 **컴파일 타임**에 직접 호출한다.

### 2. 동적 다형성 vs 정적 다형성

| 항목 | 가상 함수 | CRTP |
|------|-----------|------|
| 다형성 시점 | 런타임 | 컴파일타임 |
| vtable 조회 | 있음 (~2ns/호출) | 없음 |
| vptr 메모리 | +8바이트 | 없음 |
| 이질 컨테이너 | 가능 (포인터) | 불가 |
| 인라인 최적화 | 어려움 | 컴파일러 최적화 가능 |

### 3. CRTP Mixin 패턴

```cpp
// <만 구현하면 >, ==, <=, >= 자동 생성
template <typename Derived>
class Comparable {
    const Derived& self() const { return *static_cast<const Derived*>(this); }
public:
    bool operator<(const Derived& rhs) const { return self().value() < rhs.value(); }
    bool operator>(const Derived& rhs) const { return rhs < self(); }
    bool operator==(const Derived& rhs) const {
        return !(self() < rhs) && !(rhs < self());
    }
};

class Score : public Comparable<Score> {
    int v_;
public:
    Score(int v) : v_(v) {}
    int value() const { return v_; }
};
```

### 4. 정책 기반 설계 (Policy-based Design)

```cpp
template <
    typename T,
    template <typename> class SortPolicy  = AscendingSort,
    template <typename> class PrintPolicy = CommaFormat
>
class Container
    : private SortPolicy<std::vector<T>>
    , private PrintPolicy<std::vector<T>>
{ /* ... */ };

// 컴파일 타임에 정책 조합
Container<int>                                    c1; // 오름차순 + 콤마
Container<int, DescendingSort, LineFormat>        c2; // 내림차순 + 줄바꿈
```

### 5. CRTP 주의사항

- 기반 클래스 포인터(`Base<X>*`)로는 다른 파생 클래스들을 하나의 컨테이너에 담기 어렵다.
- 파생 클래스가 요구 메서드를 구현하지 않으면 **컴파일 오류** 발생.
- `virtual` 없이 컴파일 타임 다형성 → 임베디드·고성능 코드에 유리.

---

## 과제 개요

| 파일 | 내용 |
|------|------|
| `homework.cpp`  | CRTP 도형 계층 + Comparable Mixin 구현 |
| `homework2.cpp` | 정책 기반 컨테이너 + Singleton & Clone CRTP |
| `game1.cpp`     | CRTP 전사 계층을 이용한 텍스트 RPG 전투 |

---

## 컴파일

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw1  && ./hw1
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2  && ./hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game && ./game
```
