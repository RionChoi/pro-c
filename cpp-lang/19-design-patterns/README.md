# C++-19: 디자인 패턴 (Design Patterns)

## 개요
디자인 패턴은 소프트웨어 설계에서 자주 등장하는 문제에 대한 재사용 가능한 해결책입니다. GoF(Gang of Four)가 정립한 23가지 패턴 중 이 세션에서는 가장 실용적인 세 가지를 학습합니다.

## 주요 개념

### 1. Singleton 패턴 (생성 패턴)
- 클래스의 인스턴스가 **단 하나만** 존재하도록 보장합니다.
- 전역 접근점을 제공합니다.
- 활용 예: 설정 관리자, 로그 시스템, DB 연결 풀

```cpp
class Config {
public:
    static Config& getInstance() {
        static Config instance;  // 최초 호출 시 한 번만 생성
        return instance;
    }
private:
    Config() {}
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};
```

### 2. Observer 패턴 (행동 패턴)
- 객체의 상태 변화를 **다수의 관찰자(Observer)**에게 자동으로 알립니다.
- Subject(발행자)와 Observer(구독자)를 느슨하게 결합합니다.
- 활용 예: 이벤트 시스템, GUI 콜백, 주식 가격 알림

### 3. Strategy 패턴 (행동 패턴)
- 알고리즘 군을 정의하고, 각각을 캡슐화하여 **런타임에 교체 가능**하게 합니다.
- `if-else` / `switch` 분기를 제거하고 확장성을 높입니다.
- 활용 예: 정렬 알고리즘 선택, 결제 방식, 게임 캐릭터 공격 전략

## 과제 목표
| 파일 | 패턴 | 내용 |
|------|------|------|
| `homework.cpp` | Singleton | 전역 설정 관리자 구현 |
| `homework2.cpp` | Observer | 주식 가격 알림 시스템 |
| `game1.cpp` | Strategy | 전사 캐릭터 전투 전략 게임 |

## 핵심 원칙
- **개방-폐쇄 원칙(OCP)**: 확장에는 열려 있고, 수정에는 닫혀 있어야 한다.
- **단일 책임 원칙(SRP)**: 클래스는 하나의 책임만 가져야 한다.
- **의존성 역전 원칙(DIP)**: 구체적인 구현이 아닌 추상화에 의존해야 한다.
