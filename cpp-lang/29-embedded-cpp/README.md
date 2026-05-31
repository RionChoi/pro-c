# Session 29: 임베디드 C++ (Embedded C++)

## 학습 목표

- 임베디드 시스템에서 사용하는 C++ 패턴을 이해한다
- 비트 조작과 타입 안전 레지스터 접근을 구현한다
- 고정소수점 산술로 부동소수점 없이 수치 연산을 처리한다
- RAII로 임계 구역(Critical Section)을 안전하게 관리한다
- 상태 머신(State Machine) 패턴을 임베디드 컨텍스트에 적용한다

## 핵심 개념

### volatile 키워드
하드웨어 레지스터는 컴파일러가 임의로 최적화(캐싱)하면 안 된다.
`volatile`을 사용하면 매 접근마다 실제 메모리를 읽고 쓴다.

```cpp
volatile uint8_t* GPIO_DIR = reinterpret_cast<volatile uint8_t*>(0x40020000);
```

### 비트 조작 레지스터
```cpp
value |=  (1U << bit);  // 비트 SET
value &= ~(1U << bit);  // 비트 CLEAR
value ^=  (1U << bit);  // 비트 TOGGLE
bool v = (value >> bit) & 1U;  // 비트 READ
```

### std::expected (C++23)
임베디드에서는 예외(exception)를 비활성화하는 경우가 많다.
`std::expected<T, E>`는 예외 없이 오류를 값으로 전달한다.

```cpp
std::expected<bool, GpioError> read(uint8_t pin);

if (auto r = gpio.read(pin); r)
    use(*r);
else
    handle_error(r.error());
```

### 고정소수점 산술
임베디드에서 FPU(부동소수점 처리 장치)가 없는 경우,
정수 타입으로 소수를 표현한다 (Q 포맷).

```
Q8.8: int16_t, 상위 8비트=정수부, 하위 8비트=소수부
      1.5 → 0x0180 (= 384 = 1.5 × 256)
```

### RAII 임계 구역
인터럽트를 비활성화해야 하는 코드 블록을 RAII로 보호한다.
블록을 벗어나면 자동으로 인터럽트가 복원된다.

```cpp
{
    CriticalSection cs;   // 인터럽트 비활성화
    shared_data = value;  // 안전한 접근
}   // 소멸자에서 인터럽트 자동 복원
```

### 상태 머신
임베디드 시스템의 제어 로직은 상태 머신으로 표현한다.
각 상태는 입력에 따라 다른 상태로 전이한다.

```
RED → RED+YELLOW → GREEN → YELLOW → RED
```

## 파일 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | GPIO 레지스터 시뮬레이터 (비트 조작 + std::expected) |
| `homework2.cpp` | 고정소수점 산술 + RAII 임계 구역 |
| `game1.cpp` | 신호등 상태 머신 시뮬레이터 |

## 컴파일

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o homework
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o homework2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game1
```

## 참고

- Bro Code: C++ Full Course (임베디드 패턴 응용)
- ISO C++ Embedded FAQ: volatile, constexpr, RAII
- MISRA C++: 임베디드 안전 코딩 가이드라인
