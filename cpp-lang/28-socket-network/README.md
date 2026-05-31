# 세션 28: 소켓 네트워크 프로그래밍 (Socket Network Programming)

## 학습 목표

1. 소켓(Socket)의 개념과 TCP/IP 통신 원리를 이해한다.
2. POSIX 소켓 API를 C++에서 사용하는 방법을 익힌다.
3. 서버-클라이언트 구조를 설계하고 구현할 수 있다.
4. `fork()`를 이용해 다중 클라이언트를 동시에 처리하는 Prefork 서버를 구현한다.

## 핵심 개념

### 소켓(Socket)이란?
- 네트워크 통신의 **끝점(endpoint)**
- IP 주소 + 포트 번호로 고유하게 식별
- POSIX에서 파일 디스크립터처럼 `int` 타입으로 사용

### 주요 소켓 함수

| 함수 | 역할 |
|------|------|
| `socket(domain, type, proto)` | 소켓 생성 (파일 디스크립터 반환) |
| `bind(fd, addr, len)` | 소켓에 IP 주소와 포트 번호 연결 |
| `listen(fd, backlog)` | 연결 대기 큐 설정 (서버 전용) |
| `accept(fd, addr, len)` | 클라이언트 연결 수락 (새 fd 반환) |
| `connect(fd, addr, len)` | 서버에 연결 (클라이언트 전용) |
| `send(fd, buf, len, flags)` | 데이터 전송 |
| `recv(fd, buf, len, flags)` | 데이터 수신 |
| `close(fd)` | 소켓 닫기 |

### TCP 서버 흐름
```
socket() → bind() → listen() → accept() → recv()/send() → close()
```

### TCP 클라이언트 흐름
```
socket() → connect() → send()/recv() → close()
```

### 주요 자료구조
```cpp
sockaddr_in addr{};
addr.sin_family      = AF_INET;                     // IPv4
addr.sin_addr.s_addr = INADDR_ANY;                  // 모든 인터페이스
addr.sin_port        = htons(static_cast<uint16_t>(PORT)); // 포트 (네트워크 바이트 순서)
```

### 바이트 순서 변환
| 함수 | 설명 |
|------|------|
| `htons(x)` | host → network 16비트 (포트 설정 시 사용) |
| `htonl(x)` | host → network 32비트 (IP 설정 시 사용) |
| `ntohs(x)` | network → host 16비트 |
| `ntohl(x)` | network → host 32비트 |

### Prefork 서버 패턴
- `accept()` 후 `fork()`로 자식 프로세스 생성
- 자식: 클라이언트 처리 후 `_exit(0)`
- 부모: 자식 pid를 저장 후 다음 연결 대기
- 루프 종료 후 `waitpid()`로 모든 자식 회수

## 과제

### homework.cpp
TCP Echo 서버/클라이언트를 단일 파일로 구현한다.
- `fork()`로 서버(부모)와 클라이언트(자식) 분리
- 클라이언트는 3개의 메시지를 전송하고 에코 응답 출력
- 핵심 소켓 API(socket, bind, listen, accept, connect, send, recv)를 직접 사용

### homework2.cpp
Prefork 방식 에코 서버를 구현한다.
- `accept()` 후 `fork()`로 클라이언트마다 자식 프로세스 생성
- 3개의 클라이언트 프로세스가 동시에 접속하는 시나리오
- `waitpid()`로 좀비 프로세스 방지

### game1.cpp
네트워크 숫자 맞추기 게임을 구현한다.
- 서버: 1~100 사이 랜덤 숫자 생성, UP/DOWN 힌트 제공 (최대 7번)
- 클라이언트: **이진 탐색(Binary Search)** 전략으로 최적 추측
- 수학적 근거: log₂(100) ≈ 6.64 → 7번 안에 반드시 정답

## 컴파일

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw28   && ./hw28
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw28_2 && ./hw28_2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game28 && ./game28
```
