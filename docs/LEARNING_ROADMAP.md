# pro-c 학습 로드맵 (C → C++ 통합 과정)

## 목표
- C 문법 기초부터 고급까지 체계적으로 습득한다.
- C 완료 후 C++를 이어서 학습한다.
- 매일 아침 7시(KST) 자동으로 2세션씩 코딩하고 GitHub에 푸시한다.
- 모든 28세션 완료 시 프로젝트를 종료한다.

## 참조 강좌
| 언어 | 강좌 | 채널 |
|------|------|------|
| C | C Programming Full Course | Bro Code (YouTube) |
| C++ | C++ Full Course | Bro Code (YouTube) |

---

## 세션별 파일 구성
| 파일 | 역할 |
|------|------|
| `README.md` | 학습 내용 요약 (한국어) |
| `homework.c/.cpp` | 과제 1 — 핵심 연습 |
| `homework2.c/.cpp` | 과제 2 — 확장 연습 |
| `game1.c/.cpp` | 미니 게임 — 학습 내용 응용 |

---

## Phase 1: C 언어 (c-lang/) — 20세션

### 완료된 세션

| 세션 | 주제 | 상태 |
|------|------|------|
| 01 | 변수와 입출력 (Variables & I/O) | ✅ 완료 |
| 02 | 조건문과 반복문 (Conditions & Loops) | ✅ 완료 |
| 03 | 함수와 배열 (Functions & Arrays) | ✅ 완료 |
| 04 | 포인터와 문자열 (Pointers & Strings) | ✅ 완료 |
| 05 | 구조체 (Structs) | ✅ 완료 |
| 06 | Enum & Typedef | ✅ 완료 |
| 07 | 동적 메모리 (Dynamic Memory) | ✅ 완료 |
| 08 | 파일 I/O (File I/O) | ✅ 완료 |
| 09 | 전처리기 & 헤더 (Preprocessor & Headers) | ✅ 완료 |
| 10 | 비트연산 & 고급 (Bitwise & Advanced) | ✅ 완료 |
| 11 | 연결 리스트 (Linked List) | ✅ 완료 |
| 12 | 정렬 & 탐색 (Sorting & Searching) | ✅ 완료 |
| 13 | 에러 처리 (Error Handling) | ✅ 완료 |
| 14 | 스택 & 큐 (Stack & Queue) | ✅ 완료 |
| 15 | 재귀 (Recursion) | ✅ 완료 |
| 16 | 함수 포인터 & 콜백 (Function Pointers) | ✅ 완료 |
| 17 | 멀티파일 프로젝트 (Multi-file Project) | ✅ 완료 |
| 18 | 커맨드라인 인자 (Command-line Args) | ✅ 완료 |
| 19 | 해시 테이블 (Hash Table) | ✅ 완료 |
| 20 | C 최종 프로젝트 (Final Project) | ✅ 완료 |

### 남은 세션

(없음 — C 언어 20세션 완료!)

---

## Phase 2: C++ (cpp-lang/) — 20세션

> C 언어 20세션 완료 후 시작

| 세션 | 주제 | 상태 |
|------|------|------|
| 01 | C++ 기초 & iostream | ✅ 완료 |
| 02 | 문자열 & 네임스페이스 (String & Namespace) | ✅ 완료 |
| 03 | 제어 흐름 (Control Flow) | ✅ 완료 |
| 04 | 함수 & 오버로딩 (Functions & Overloading) | ✅ 완료 |
| 05 | 배열 & 벡터 (Arrays & Vectors) | ✅ 완료 |
| 06 | 포인터 & 참조 (Pointers & References) | ✅ 완료 |
     72| 07 | OOP — 클래스 (Classes) | ✅ 완료 |
     73|| 08 | 생성자 & 소멸자 (Constructors & Destructors) | ✅ 완료 |
| 09 | 상속 & 다형성 (Inheritance & Polymorphism) | ⬜ 대기 |
| 10 | 연산자 오버로딩 (Operator Overloading) | ⬜ 대기 |
| 11 | 템플릿 (Templates) | ⬜ 대기 |
| 12 | STL 컨테이너 (STL Containers) | ⬜ 대기 |
| 13 | STL 알고리즘 (STL Algorithms) | ⬜ 대기 |
| 14 | 예외 처리 (Exception Handling) | ⬜ 대기 |
| 15 | 파일 I/O & 스트림 (File I/O & Streams) | ⬜ 대기 |
| 16 | 람다 & 함수 객체 (Lambda & Functors) | ⬜ 대기 |
| 17 | 스마트 포인터 & 이동 시맨틱 (Smart Pointers & Move) | ⬜ 대기 |
| 18 | 멀티스레딩 기초 (Multithreading Basics) | ⬜ 대기 |
| 19 | 디자인 패턴 (Design Patterns) | ⬜ 대기 |
| 20 | C++ 최종 프로젝트 (Final Project) | ⬜ 대기 |

---

## 자동 스케줄 정보

- **실행 시간**: 매일 07:00 KST (UTC 22:00)
- **일일 분량**: 2세션
- **예상 소요**: 약 14일 (2세션/일, 총 28세션 기준)
- **자동화**: Hermes Agent cron job (`pro-c-daily-learning`)
- **완료 조건**: 28세션 전부 완료 시 프로젝트 종료

---

## 진행 기록

| 날짜 | 완료 세션 | 비고 |
|------|-----------|------|
| 2026-04-10 | C-01 ~ C-04 | 초기 세션 완료 |
| 2026-04-19 | C-05, C-06 | 구조체, Enum & Typedef |
| 2026-04-21 | C-07, C-08 | 동적 메모리, 파일 I/O |
| 2026-04-22 | C-09, C-10 | 전처리기/헤더, 비트연산 |
| 2026-04-23 | C-11, C-12 | 연결 리스트, 정렬 & 탐색 |
| 2026-04-23 | C-13, C-14 | 에러 처리, 스택 & 큐 |
| 2026-04-24 | C-15, C-16 | 재귀, 함수 포인터 & 콜백 |
| 2026-04-25 | C-17, C-18 | 멀티파일 프로젝트, 커맨드라인 인자 |
| 2026-04-26 | C-19, C-20 | 해시 테이블, C 최종 프로젝트 (주소록) |
| 2026-04-27 | C++-01, C++-02 | C++ 기초 입출력, 문자열 & 네임스페이스 |
| 2026-04-28 | C++-03, C++-04 | 제어 흐름, 함수 & 오버로딩 |
   114|| 2026-04-29 | C++-05, C++-06 | 배열 & 벡터, 포인터 & 참조 |
   115|| 2026-04-30 | C++-07, C++-08 | OOP - 클래스, 생성자 & 소멸자 |
