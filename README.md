# pro-c — C / C++ 학습 프로젝트

C 언어 기초부터 고급까지 학습하고, 이어서 C++까지 마친 학습 프로젝트입니다.
로드맵 기준으로 총 40세션이 모두 완료되었습니다.

## 참조 강좌
- **C**: [Bro Code — C Programming Full Course](https://youtube.com/watch?v=87SH2Cn0s9A)
- **C++**: [Bro Code — C++ Full Course](https://youtube.com/watch?v=uhFpPlMsLzY)

## 완료 현황

### Phase 1: C 언어 (`c-lang/`) — 20세션

| 세션 | 주제 | 상태 |
|------|------|------|
| 01 | 변수와 입출력 | ✅ |
| 02 | 조건문과 반복문 | ✅ |
| 03 | 함수와 배열 | ✅ |
| 04 | 포인터와 문자열 | ✅ |
| 05 | 구조체 | ✅ |
| 06 | Enum & Typedef | ✅ |
| 07 | 동적 메모리 | ✅ |
| 08 | 파일 I/O | ✅ |
| 09 | 전처리기 & 헤더 | ✅ |
| 10 | 비트연산 & 고급 | ✅ |
| 11 | 연결 리스트 | ✅ |
| 12 | 정렬 & 탐색 | ✅ |
| 13 | 에러 처리 | ✅ |
| 14 | 스택 & 큐 | ✅ |
| 15 | 재귀 | ✅ |
| 16 | 함수 포인터 & 콜백 | ✅ |
| 17 | 멀티파일 프로젝트 | ✅ |
| 18 | 커맨드라인 인자 | ✅ |
| 19 | 해시 테이블 | ✅ |
| 20 | C 최종 프로젝트 | ✅ |

### Phase 2: C++ (`cpp-lang/`) — 20세션

| 세션 | 주제 | 상태 |
|------|------|------|
| 01 | C++ 기초 & iostream | ✅ |
| 02 | 문자열 & 네임스페이스 | ✅ |
| 03 | 제어 흐름 | ✅ |
| 04 | 함수 & 오버로딩 | ✅ |
| 05 | 배열 & 벡터 | ✅ |
| 06 | 포인터 & 참조 | ✅ |
| 07 | OOP — 클래스 | ✅ |
| 08 | 생성자 & 소멸자 | ✅ |
| 09 | 상속 & 다형성 | ✅ |
| 10 | 연산자 오버로딩 | ✅ |
| 11 | 템플릿 | ✅ |
| 12 | STL 컨테이너 | ✅ |
| 13 | STL 알고리즘 | ✅ |
| 14 | 예외 처리 | ✅ |
| 15 | 파일 I/O & 스트림 | ✅ |
| 16 | 람다 & 함수 객체 | ✅ |
| 17 | 스마트 포인터 & 이동 시맨틱 | ✅ |
| 18 | 멀티스레딩 기초 | ✅ |
| 19 | 디자인 패턴 | ✅ |
| 20 | C++ 최종 프로젝트 | ✅ |

## 세션별 파일 구성

| 파일 | 역할 |
|------|------|
| `README.md` | 학습 내용 요약 (한국어) |
| `homework.c/.cpp` | 과제 1 |
| `homework2.c/.cpp` | 과제 2 |
| `game1.c/.cpp` | 미니 게임 |

## 프로젝트 구조

```text
pro-c/
├── README.md
├── CLAUDE.md
├── docs/
│   ├── LEARNING_ROADMAP.md
│   ├── CODE_REVIEW.md
│   └── SCHEDULE.md
├── c-lang/                  # C 20세션
│   ├── 01-variables-io/
│   ├── ...
│   └── 20-final-project/
└── cpp-lang/                # C++ 20세션
    ├── 01-cpp-basics-iostream/
    ├── ...
    └── 20-final-project/
```

## 실행 방법

### C 코드
```bash
cc -Wall -Wextra -Wpedantic c-lang/05-structs/homework.c -o hw
./hw
```

### C++ 코드
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/01-cpp-basics-iostream/homework.cpp -o hw
./hw
```

## 자동화
- **Hermes Agent** cron job으로 매일 아침 7시(KST) 자동 실행해 세션을 생성
- 총 40세션으로 구성되며, 로드맵 기준 전체 완료
- 상세 설정: [`docs/SCHEDULE.md`](docs/SCHEDULE.md)
