# C++ 세션 18: 멀티스레딩 기초 (Multithreading Basics)

## 학습 목표
- 멀티스레딩의 개념과 필요성을 이해합니다.
- C++11 표준 라이브러리의 `std::thread`를 사용하여 스레드를 생성하고 관리합니다.
- `std::mutex`와 `std::lock_guard`를 사용하여 공유 자원에 대한 접근을 동기화하고 Race Condition을 방지합니다.
- `std::async`와 `std::future`를 사용하여 비동기 작업을 처리하는 방법을 학습합니다.
- 데드락(Deadlock)의 개념을 이해하고 피하는 방법을 알아봅니다.

## 핵심 개념

### 멀티스레딩 (Multithreading)
멀티스레딩은 하나의 프로그램 내에서 여러 코드를 동시에 실행할 수 있도록 하는 기법입니다. 이를 통해 CPU 코어를 효율적으로 활용하고, 프로그램의 응답성을 향상시키며, 복잡한 작업을 병렬로 처리할 수 있습니다.

- **스레드 (Thread)**: 운영체제가 독립적으로 스케줄링할 수 있는 가장 작은 실행 단위입니다. 모든 스레드는 프로세스의 메모리 공간을 공유합니다.
- **Race Condition**: 여러 스레드가 동시에 공유 데이터를 접근하여 변경할 때, 접근 순서에 따라 실행 결과가 달라지는 오류를 의미합니다. 이는 예측 불가능한 버그로 이어질 수 있습니다.
- **데드락 (Deadlock)**: 두 개 이상의 스레드가 서로 상대방이 점유하고 있는 자원을 기다리면서 무한히 대기하는 상태를 말합니다.

### `std::thread`
C++11 표준 라이브러리에서 스레드를 생성하고 관리하는 기본 클래스입니다. 함수 객체, 람다 표현식, 멤버 함수 등을 인자로 받아 새로운 스레드에서 실행할 수 있습니다.

```cpp
#include <thread>

void function_to_run_in_thread() {
    // ...
}

int main() {
    std::thread t(function_to_run_in_thread);
    t.join(); // 메인 스레드가 t의 완료를 기다림
    // t.detach(); // t를 백그라운드에서 실행하고 메인 스레드와 분리
    return 0;
}
```

### `std::mutex`와 `std::lock_guard`
공유 자원에 대한 동시 접근을 제어하여 Race Condition을 방지하기 위한 동기화 메커니즘입니다.

- **`std::mutex`**: 상호 배제(mutual exclusion) 객체로, 한 번에 하나의 스레드만 락을 소유할 수 있도록 합니다. 공유 자원에 접근하기 전에 락을 얻고, 접근 후에는 락을 해제해야 합니다.
- **`std::lock_guard`**: RAII 원칙에 따라 뮤텍스를 안전하게 관리하는 락 래퍼(lock wrapper)입니다. `lock_guard` 객체가 생성될 때 뮤텍스를 락하고, 스코프를 벗어날 때 자동으로 언락합니다. `lock_guard`는 데드락 방지에는 제한적입니다.

```cpp
#include <mutex>
#include <iostream>

std::mutex mtx; // 전역 또는 클래스 멤버 뮤텍스
int shared_data = 0;

void increment_shared_data() {
    std::lock_guard<std::mutex> lock(mtx); // 스코프 진입 시 락, 스코프 탈출 시 언락
    shared_data++;
    std::cout << "Shared data: " << shared_data << "\n";
}
```

### `std::async`와 `std::future`
비동기적으로 함수를 실행하고 그 결과를 얻는 현대적인 방법입니다. `std::async`는 비동기 작업을 시작하고, `std::future`는 해당 작업의 결과를 나중에 검색할 수 있는 핸들(handle)을 제공합니다.

```cpp
#include <future>
#include <iostream>

int calculate_sum(int a, int b) {
    // ... 복잡한 계산 ...
    return a + b;
}

int main() {
    // 비동기적으로 calculate_sum 실행
    std::future<int> result_future = std::async(std::launch::async, calculate_sum, 10, 20);

    // 다른 작업 수행...

    // 결과가 준비될 때까지 기다리고 결과 가져오기
    int sum = result_future.get();
    std::cout << "Sum: " << sum << "\n";
    return 0;
}
```

## 과제 목표
- `homework.cpp`: `std::thread`를 사용하여 여러 스레드를 생성하고, 각 스레드가 독립적인 작업을 수행하도록 합니다. `join()`과 `detach()`의 사용법을 시연합니다.
- `homework2.cpp`: `std::mutex`와 `std::lock_guard`를 사용하여 공유 자원(카운터)에 대한 여러 스레드의 동시 접근을 안전하게 관리하고 Race Condition이 발생하지 않음을 증명합니다.
- `game1.cpp`: 간단한 게임 시나리오에서 `std::async`와 `std::future`를 사용하여 비동기 작업을 처리하는 로직을 구현합니다 (예: 게임 점수 계산, 자원 로딩 등).

## 컴파일 및 실행
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion -pthread homework.cpp -o homework
./homework

g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion -pthread homework2.cpp -o homework2
./homework2

g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion -pthread game1.cpp -o game1
./game1
```

**주의**: 멀티스레딩 프로그램을 컴파일할 때는 `-pthread` 또는 `-lpthread` 플래그를 추가해야 합니다.
