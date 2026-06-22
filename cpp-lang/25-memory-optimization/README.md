# 세션 25: 메모리 최적화 & 캐시 (Memory Optimization & Cache)

## 학습 목표

- CPU 캐시 계층 구조(L1/L2/L3)를 이해하고 캐시 친화적 코드를 작성한다.
- 메모리 정렬(alignment)과 패딩(padding)이 성능에 미치는 영향을 파악한다.
- 데이터 지역성(data locality) 원칙을 적용하여 캐시 미스를 줄인다.
- 메모리 풀(Memory Pool)과 오브젝트 풀(Object Pool) 패턴을 구현한다.
- `alignas`, `alignof`, `std::aligned_storage` 등 C++ 정렬 도구를 활용한다.

---

## 핵심 개념

### 1. CPU 캐시 계층

| 캐시 | 크기 | 접근 지연 |
|------|------|-----------|
| L1   | ~32KB | ~4 사이클 |
| L2   | ~256KB | ~12 사이클 |
| L3   | ~8MB | ~40 사이클 |
| RAM  | GB 단위 | ~200 사이클 |

캐시 라인(cache line)은 보통 **64바이트**. 한 번에 64바이트씩 캐시로 로드된다.

### 2. 캐시 친화적 접근 패턴

```cpp
// 나쁜 예 — 열(column) 방향 접근: 캐시 미스 다발
for (int j = 0; j < N; ++j)
    for (int i = 0; i < N; ++i)
        sum += matrix[i][j];

// 좋은 예 — 행(row) 방향 접근: 공간적 지역성 활용
for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
        sum += matrix[i][j];
```

### 3. 구조체 패딩 & 정렬

컴파일러는 멤버를 자연 정렬(natural alignment)에 맞추기 위해 빈 공간(패딩)을 삽입한다.

```cpp
struct Bad {   // 12바이트 (패딩 낭비)
    char  a;   // 1 + 3 패딩
    int   b;   // 4
    char  c;   // 1 + 3 패딩
};

struct Good {  // 8바이트 (패딩 최소화)
    int   b;   // 4
    char  a;   // 1
    char  c;   // 1 + 2 패딩
};
```

### 4. `alignas` / `alignof`

```cpp
alignas(64) float cache_line_data[16]; // 캐시 라인 경계에 정렬
std::cout << alignof(double);          // double의 정렬 요구사항 출력
```

### 5. 메모리 풀 (Memory Pool)

빈번한 `new`/`delete`는 힙 단편화(fragmentation)와 할당 비용을 유발한다.
메모리 풀은 큰 블록을 미리 할당해 두고, 작은 객체를 O(1)에 공급한다.

```cpp
class MemoryPool {
    std::byte  buffer[POOL_SIZE];
    std::size_t offset = 0;
public:
    void* allocate(std::size_t size, std::size_t align) {
        auto ptr = std::align(align, size,
                              (void*)(buffer + offset),
                              POOL_SIZE - offset);
        if (ptr) offset += size;
        return ptr;
    }
    void reset() { offset = 0; }
};
```

### 6. False Sharing

서로 다른 스레드가 같은 캐시 라인에 있는 **다른** 변수를 수정하면
캐시 무효화(invalidation)가 발생해 성능이 크게 저하된다.

```cpp
// 나쁜 예 — counter_a와 counter_b가 같은 캐시 라인에 있을 수 있음
struct Counters { int a, b; };

// 좋은 예 — 각 카운터를 캐시 라인 크기로 패딩
struct alignas(64) PaddedCounter { int value; };
```

---

## 과제 개요

| 파일 | 내용 |
|------|------|
| `homework.cpp`  | 구조체 레이아웃 분석 + 메모리 풀 구현 |
| `homework2.cpp` | 캐시 미스 비교 벤치마크 (행/열 접근) |
| `game1.cpp`     | 오브젝트 풀을 이용한 파티클 시뮬레이터 |

---

## 컴파일

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic -O2 homework.cpp  -o hw1  && ./hw1
g++ -std=c++23 -Wall -Wextra -Wpedantic -O2 homework2.cpp -o hw2  && ./hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic -O2 game1.cpp     -o game && ./game
```
