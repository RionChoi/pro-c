# C++ 세션 17: 스마트 포인터 & 이동 시맨틱 (Smart Pointers & Move Semantics)

## 학습 목표
- C++ 스마트 포인터의 개념과 필요성을 이해합니다.
- `std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`의 사용법과 차이점을 숙지합니다.
- 이동 시맨틱(Move Semantics)의 개념과 `std::move`의 활용법을 학습합니다.
- 이동 생성자 및 이동 할당 연산자를 구현하여 리소스 관리 효율성을 높입니다.
- RAII(Resource Acquisition Is Initialization) 원칙을 스마트 포인터와 이동 시맨틱에 적용합니다.

## 핵심 개념

### 스마트 포인터 (Smart Pointers)
C++11에서 도입된 스마트 포인터는 동적으로 할당된 메모리를 자동으로 관리하여 메모리 누수와 Dangling Pointer 문제를 방지합니다. RAII 원칙에 따라, 객체가 스코프를 벗어날 때 자동으로 메모리를 해제합니다.

- **`std::unique_ptr`**: 독점적인 소유권을 가집니다. 한 시점에 오직 하나의 `unique_ptr`만이 특정 리소스를 소유할 수 있습니다. 복사될 수 없지만, `std::move`를 통해 소유권을 이동할 수 있습니다.
- **`std::shared_ptr`**: 공유 소유권을 가집니다. 여러 `shared_ptr`가 동일한 리소스를 소유할 수 있으며, 참조 카운팅(reference counting)을 통해 모든 `shared_ptr`가 소멸될 때 리소스를 해제합니다.
- **`std::weak_ptr`**: `shared_ptr`의 순환 참조(circular reference) 문제를 해결하기 위해 사용됩니다. 리소스의 소유권을 가지지 않고, `shared_ptr`를 감시합니다. `shared_ptr`가 소멸되면 `weak_ptr`는 만료됩니다.

### 이동 시맨틱 (Move Semantics)
C++11에서 도입된 이동 시맨틱은 불필요한 복사(copy) 작업을 줄이고 리소스(메모리, 파일 핸들 등)를 효율적으로 이동(move)시키는 기법입니다. 임시 객체나 더 이상 필요 없는 객체의 리소스를 '훔쳐와서' 새 객체에 할당함으로써 성능을 최적화합니다.

- **`std::move`**: 객체를 rvalue 레퍼런스(rvalue reference)로 캐스팅하여 이동 가능(movable)하다는 것을 컴파일러에게 알려줍니다. 실제 이동을 수행하는 것이 아니라, 이동 생성자나 이동 할당 연산자가 호출될 수 있도록 합니다.
- **이동 생성자 (Move Constructor)**: `T::T(T&& other)` 형태. `other` 객체의 리소스를 현재 객체로 이동시키고, `other` 객체는 유효하지만(valid) 지정되지 않은(unspecified) 상태로 만듭니다.
- **이동 할당 연산자 (Move Assignment Operator)**: `T& T::operator=(T&& other)` 형태. 이동 생성자와 유사하게 리소스를 이동시킵니다.

## 과제 목표
- `homework.cpp`: `unique_ptr`와 `shared_ptr`의 기본 사용법과 소유권 관리 방식을 구현합니다.
- `homework2.cpp`: 사용자 정의 클래스에 이동 생성자와 이동 할당 연산자를 구현하고, `std::move`를 활용하여 리소스 이동을 시연합니다.
- `game1.cpp`: 스마트 포인터를 사용하여 동적으로 생성된 게임 오브젝트를 관리하는 간단한 게임 로직을 구현합니다.

## 컴파일 및 실행
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion homework.cpp -o homework
./homework

g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion homework2.cpp -o homework2
./homework2

g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion game1.cpp -o game1
./game1
```
