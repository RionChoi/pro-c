# Session 39: C++ DSL 설계 (Fluent Interface & Builder)

## 학습 목표
- Domain Specific Language (DSL) 설계 패턴
- Fluent Interface 기법 (메서드 체이닝)
- Builder 패턴 구현
- 읽기 쉬운 선언적 API 만들기
- 타입 안전성 유지

## 핵심 개념

### Fluent Interface
메서드 체이닝으로 자연스러운 문법 구현:
```cpp
Query q = select("name", "age")
    .from("users")
    .where("age > 18")
    .orderBy("name")
    .limit(10);
```

### Builder 패턴
복잡한 객체를 단계적으로 구성:
```cpp
HTTPRequest req = HTTPRequest::builder()
    .url("https://example.com")
    .method("GET")
    .header("User-Agent", "MyApp")
    .timeout(5000)
    .build();
```

### 선언적 프로그래밍
명령형(how)이 아닌 선언형(what) 스타일.

## 과제

### homework.cpp
**SQL 쿼리 빌더**
- SELECT, FROM, WHERE, ORDER BY, LIMIT 메서드
- 메서드 체이닝 지원
- 쿼리 문자열 생성

예:
```cpp
auto q = Query()
    .select("id", "name")
    .from("employees")
    .where("salary > 50000")
    .orderBy("name ASC")
    .limit(5);
q.print();  // SELECT id, name FROM employees WHERE salary > 50000 ORDER BY name ASC LIMIT 5
```

### homework2.cpp
**HTTP 요청 빌더**
- URL 설정
- HTTP 메서드 (GET, POST, PUT, DELETE)
- 헤더 추가
- 바디 설정
- 타임아웃 설정
- build() 메서드로 최종 생성

예:
```cpp
HTTPRequest req = HTTPRequest::builder()
    .url("https://api.example.com/users")
    .method("POST")
    .header("Content-Type", "application/json")
    .body("{\"name\": \"John\", \"age\": 30}")
    .timeout(5000)
    .build();
req.print();
```

### game1.cpp
**게임 캐릭터 생성기 (DSL)**
- 캐릭터 이름, 직업, 스킬, 장비 설정
- Fluent Interface로 자연스러운 문법
- 최종 캐릭터 출력

예:
```cpp
Character hero = Character::build()
    .withName("Aragorn")
    .ofClass("Warrior")
    .withHealth(100)
    .addSkill("Sword Mastery", 90)
    .addSkill("Leadership", 80)
    .equip("Iron Sword")
    .equip("Steel Shield")
    .create();
hero.display();
```

## 컴파일
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw39
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw39_2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game39
```

## 주의사항
- 메서드 체이닝: 각 메서드는 `*this` 또는 새로운 객체 반환
- 선택적 파라미터: default 값 또는 벡터로 관리
- Type safety: 템플릿이나 overload로 타입 검증
