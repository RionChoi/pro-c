# 세션 33: constexpr·consteval 심화 & 컴파일 타임 프로그래밍

## 학습 목표

- `constexpr` 함수를 컴파일 타임과 런타임 양쪽에서 사용하는 방법을 이해한다
- `consteval` 즉시 함수(immediate function)와 `constexpr`의 차이를 구분한다
- `constinit`으로 전역 변수의 컴파일 타임 초기화를 보장하는 방법을 익힌다
- `if constexpr`로 타입에 따른 컴파일 타임 분기를 작성한다
- `if consteval` (C++23)로 컴파일 타임/런타임 동작을 분리한다
- 컴파일 타임 자료구조(`std::array` 기반 테이블, 버블 정렬, 이진 탐색)를 구현한다

## 핵심 개념

### constexpr 함수

컴파일 타임과 런타임 모두에서 호출 가능하다.  
`constexpr` 변수에 결과를 저장하면 컴파일 타임에 평가된다.

```cpp
constexpr long long factorial(int n) {
    return (n <= 1) ? 1LL : static_cast<long long>(n) * factorial(n - 1);
}

constexpr long long f10 = factorial(10);   // 컴파일 타임
static_assert(f10 == 3628800LL);           // 컴파일 타임 검증

int n = 5;
long long fn = factorial(n);               // 런타임
```

### consteval (C++20)

**즉시 함수(immediate function)**: 반드시 컴파일 타임에만 평가된다.  
런타임 값으로 호출하면 컴파일 오류가 발생한다.

```cpp
consteval int square(int n) { return n * n; }

constexpr int s = square(7);  // OK: 컴파일 타임
int x = 7;
int r = square(x);            // 오류: x는 런타임 값
```

### constinit (C++20)

전역 변수가 **상수 초기화**됨을 보장한다.  
`const`와 달리 런타임에 수정 가능하다. 초기식이 상수 표현식이 아니면 컴파일 오류.

```cpp
constinit int g_max = 1000;  // 컴파일 타임에 초기화됨을 보장
g_max = 2000;                // 런타임 수정은 허용
```

### if constexpr

템플릿 내 컴파일 타임 조건 분기. 선택되지 않은 분기는 코드 생성에서 제외된다.

```cpp
template<typename T>
constexpr const char* type_name() {
    if constexpr (std::is_integral_v<T>)
        return "정수형";
    else if constexpr (std::is_floating_point_v<T>)
        return "부동소수점형";
    else
        return "기타";
}
```

### if consteval (C++23)

`constexpr` 함수 안에서 **"지금 컴파일 타임으로 평가 중인가?"** 를 분기한다.  
컴파일 타임 경로에서는 `std::sqrt` 같은 비-constexpr 함수를 호출할 수 없으므로,  
직접 구현한 뉴턴-랩슨 반복법을 사용한다.

```cpp
constexpr double my_sqrt(double x) {
    if consteval {
        // 컴파일 타임: 뉴턴-랩슨 반복법
        double g = x;
        for (int i = 0; i < 50; ++i)
            g = 0.5 * (g + x / g);
        return g;
    } else {
        return std::sqrt(x);  // 런타임: 표준 라이브러리
    }
}
```

### 컴파일 타임 자료구조

C++20부터 `std::array`의 모든 멤버 함수가 `constexpr`이다.  
`std::sort`(algorithm)도 C++20부터 `constexpr`이다.

```cpp
constexpr auto make_table() {
    std::array<int, 10> arr{};
    for (int i = 0; i < 10; ++i)
        arr[static_cast<std::size_t>(i)] = (i + 1) * (i + 1);
    return arr;
}
constexpr auto TABLE = make_table();
static_assert(TABLE[3] == 16);
```

## 파일 구성

| 파일 | 역할 |
|------|------|
| `homework.cpp` | constexpr 수학 함수·테이블, consteval, constinit, if constexpr, if consteval |
| `homework2.cpp` | 컴파일 타임 해시·문자열·버블 정렬·이진 탐색·정책 기반 누산기 |
| `game1.cpp` | 컴파일 타임 소수 체·피보나치·완전제곱수 테이블을 활용한 수학 퀴즈 게임 |

## 컴파일 방법

```bash
# 기본 컴파일
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw33
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw33_2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game1

# 엄격한 컴파일 (권장)
g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework.cpp -o hw33

g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw33_2

g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion game1.cpp -o game1

./hw33
./hw33_2
./game1
```

## 핵심 정리

| 키워드 | 평가 시점 | const? | 사용 위치 |
|--------|-----------|--------|-----------|
| `constexpr` 함수 | 컴파일/런타임 | - | 함수 |
| `consteval` 함수 | 컴파일 타임만 | - | 함수 |
| `constexpr` 변수 | 컴파일 타임 | 예 | 변수 |
| `constinit` 변수 | 초기화만 컴파일 타임 | 아니오 | 전역/정적 변수 |
| `if constexpr` | 컴파일 타임 분기 | - | 템플릿 함수 안 |
| `if consteval` | 컴파일 타임 분기 | - | constexpr 함수 안 |
