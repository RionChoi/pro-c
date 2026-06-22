# 세션 24 — ECS 아키텍처 (Entity-Component-System)

## 학습 목표
- Entity를 단순 ID로 표현하고 Component를 순수 데이터로 분리한다.
- System이 특정 Component 조합을 가진 Entity만 처리하도록 설계한다.
- OOP의 깊은 상속 계층 없이 조합(Composition)으로 게임 오브젝트를 구성한다.
- `std::unordered_map`, 템플릿, `typeid`로 타입 안전한 컴포넌트 저장소를 구현한다.

## 핵심 개념

### ECS 구성요소
| 요소 | 역할 | 구현 |
|------|------|------|
| **Entity** | 게임 오브젝트 식별자 | `uint32_t` ID |
| **Component** | 순수 데이터 (로직 없음) | `struct Position { float x, y; }` |
| **System** | 특정 컴포넌트를 가진 엔티티를 처리 | 함수 또는 클래스 |

### OOP vs ECS 비교
```
OOP:  Character → Warrior (상속)
ECS:  Entity 1 + Position + Health + AttackPower
      Entity 2 + Position + Health + MagicPower
```

### 컴포넌트 저장소
```cpp
// 타입별로 unordered_map에 저장
std::unordered_map<Entity, Position>  positions;
std::unordered_map<Entity, Velocity>  velocities;
std::unordered_map<Entity, Health>    healths;
```

## 과제
| 파일 | 내용 |
|------|------|
| `homework.cpp` | ECS 코어 — Entity/Component/World 구현 |
| `homework2.cpp` | System — MovementSystem, HealthSystem, 쿼리 |
| `game1.cpp` | 전투 시뮬레이터 — ECS로 구성한 RPG 전투 |

## 컴파일
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
