# Session 07: OOP — 클래스 (Classes)

## 학습 목표
- C++ 클래스의 구조(멤버 변수, 멤버 함수)를 이해한다.
- 접근 지정자(`public`, `private`, `protected`)의 차이를 파악한다.
- 클래스 내부에 함수를 정의하는 방식과 외부에서 정의하는 방식을 구분한다.
- 객체 생성 및 멤버 접근 방법을 연습한다.

## 핵심 개념

### 클래스 기본 구조
```cpp
class Student {
private:
    char name[64];
    int  age;
    double gpa;

public:
    void setInfo(const char* n, int a, double g);
    void print() const;
};
```

### 접근 지정자
| 지정자 | 설명 |
|--------|------|
| `private` | 클래스 내부에서만 접근 가능 (기본값) |
| `public` | 어디서든 접근 가능 |
| `protected` | 파생 클래스에서만 접근 가능 |

### 클래스 vs 구조체
- `struct`: 기본 접근 지정자가 `public`
- `class`: 기본 접근 지정자가 `private`

---

## 과제 1 (homework.cpp)
은행 계좌 클래스를 작성한다:
1. `private`: `accountNumber`(문자열), `balance`(double), `ownerName`(문자열)
2. `public`: `deposit()`, `withdraw()`, `display()` 멤버 함수
3. `deposit()`: 잔액 증가, `withdraw()`: 잔액 감소 (잔액 부족 시 거래 거절)
4. 메인에서 객체 2개를 생성하고 입출금 과정을 시뮬레이션한다.

---

## 과제 2 (homework2.cpp)
직사각형 클래스를 작성한다:
1. `private`: `width`, `height` (모두 double)
2. `public`: `setDimensions()`, `area()`, `perimeter()`, `isSquare()` 함수
3. `area()`: 너비×높이, `perimeter()`: 2×(너비+높이)
4. `isSquare()`: 너비와 높이가 같은지 검사
5. 3개의 직사각형 객체를 생성하고 각각 면적/둘레/정사각형 여부를 출력한다.

---

## 미니 게임 (game1.cpp) — "클래스 RPG 전투"
- 캐릭터 클래스 `Character`를 정의한다 (이름, HP, 공격력).
- `attack()`: 상대 HP를 공격력만큼 감소
- `isAlive()`: HP > 0 여부 반환
- 메인에서 플레이어와 몬스터를 생성하고 턴제 전투를 수행한다.
- `WASD`로 공격/방어/회복 중 하나를 선택 (A: 공격, D: 방어, S: 회복)
- 10턴 이내 몬스터 처치 시 클리어, 플레이어 HP가 0이 되면 게임 오버.
