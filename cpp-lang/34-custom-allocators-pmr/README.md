# Session 34: 커스텀 할당자 & std::pmr (Custom Allocators & std::pmr)

## 학습 목표
- C++ 메모리 할당 인터페이스(`std::allocator`) 이해하기
- 커스텀 할당자 구현으로 메모리 관리 전략 통제하기
- C++17 `std::pmr` (polymorphic memory resources) 활용으로 런타임 할당 전략 선택하기
- 메모리 풀, 스택 기반 할당, 추적 할당자 등 다양한 할당 정책 구현하기

## 핵심 개념
### 1. 메모리 할당자의 기본 구조
```cpp
template<typename T>
class MyAllocator {
public:
    using value_type = T;
    
    T* allocate(size_t n);
    void deallocate(T* p, size_t n);
};
```

### 2. std::pmr의 이점
- **다형성**: `std::pmr::memory_resource` 기반 클래스로 런타임 할당 전략 변경
- **성능**: 메모리 풀, 스택 할당, 스레드 로컬 할당 등으로 최적화
- **편의성**: `std::pmr::vector`, `std::pmr::string` 등에서 직접 사용

### 3. 일반적인 할당자 전략
- **기본 할당자**: `std::allocator` (new/delete 래핑)
- **메모리 풀**: 미리 할당한 메모리에서 고속 할당
- **스택 할당자**: 스택 메모리 사용 (스코프 내에서만 유효)
- **추적 할당자**: 할당/해제 통계 기록

## 할당 과제

### homework.cpp - 커스텀 할당자 구현
**목표**: 다양한 커스텀 할당자 구현으로 메모리 관리 제어

**요구사항**:
1. 기본 커스텀 할당자 (`SimpleAllocator`)
   - `allocate()`, `deallocate()` 메서드 구현
   - 할당 횟수 추적
2. 추적 할당자 (`TrackingAllocator`)
   - 총 할당량, 해제량, 활성 객체 수 기록
   - 할당/해제 로그 출력
3. `std::vector<int>`와 `std::string` 사용해서 할당자 호출 확인

### homework2.cpp - std::pmr 활용
**목표**: std::pmr으로 런타임 메모리 전략 변경

**요구사항**:
1. `std::pmr::memory_resource` 서브클래스 구현
   - 싱글톤 메모리 풀 할당자 (`PoolResource`)
   - 통계 기반 할당자 (`StatisticsResource`)
2. `std::pmr::vector<int>` 사용 (다른 리소스 전환)
3. 리소스별 성능/메모리 비교 출력

### game1.cpp - 미니 게임: 스택 기반 메모리 경쟁
**목표**: 커스텀 할당자를 활용한 높은 성능의 게임

**게임 규칙**:
1. "스택 메모리로 랜덤 숫자 생성" 게임
   - 미리 할당한 스택에서 동적 구조 생성
   - 할당/해제 속도 기록
2. 게임 결과
   - 1초 내 성공한 할당 횟수를 점수로 표시
   - 일반 할당자 vs 스택 할당자 비교

## 컴파일 & 실행

```bash
# homework.cpp 컴파일 (C++17 이상 필요)
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw34
./hw34

# homework2.cpp 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw34_2
./hw34_2

# game1.cpp 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game34
./game34
```

## 참고 자료
- C++ Reference: https://en.cppreference.com/w/cpp/memory
- std::pmr Documentation: https://en.cppreference.com/w/cpp/memory/polymorphic_allocator
- Bro Code C++ Course: Allocators and Memory Management segments
