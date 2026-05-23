# 세션 28: 소켓 네트워크 프로그래밍 (Socket Network Programming)

## 학습 목표

- BSD 소켓 API를 사용하여 TCP/UDP 통신을 구현한다.
- 서버-클라이언트 모델을 이해하고 직접 구현한다.
- `select()` / `poll()`을 이용한 I/O 멀티플렉싱을 이해한다.
- 소켓 프로그래밍의 오류 처리 패턴을 익힌다.

---

## 핵심 개념

### 1. 소켓(Socket)이란?

소켓은 네트워크 통신의 끝점(endpoint)이다. 파일 디스크립터처럼 다루며, 두 프로세스 사이의 데이터 스트림을 제공한다.

```
[클라이언트]        [서버]
socket()          socket()
connect()  <--->  bind()
                  listen()
                  accept()
send() / recv()   send() / recv()
close()           close()
```

### 2. TCP vs UDP

| 항목 | TCP | UDP |
|------|-----|-----|
| 연결 | 연결 지향 (3-way handshake) | 비연결 |
| 신뢰성 | 보장 (재전송) | 미보장 |
| 순서 | 유지 | 미보장 |
| 속도 | 느림 | 빠름 |
| 용도 | HTTP, 파일 전송 | 게임, 스트리밍 |

### 3. 주요 소켓 함수

```cpp
// 소켓 생성
int sock = socket(AF_INET, SOCK_STREAM, 0);  // TCP
int sock = socket(AF_INET, SOCK_DGRAM, 0);   // UDP

// 서버: 주소 바인딩
bind(sock, (sockaddr*)&addr, sizeof(addr));

// 서버: 연결 대기
listen(sock, BACKLOG);

// 서버: 클라이언트 수락
int client = accept(sock, nullptr, nullptr);

// 클라이언트: 서버 연결
connect(sock, (sockaddr*)&addr, sizeof(addr));

// 데이터 송수신
send(sock, buf, len, 0);
recv(sock, buf, len, 0);

// 소켓 종료
close(sock);
```

### 4. 주소 구조체

```cpp
struct sockaddr_in {
    sa_family_t    sin_family;   // AF_INET
    in_port_t      sin_port;     // htons(포트번호)
    struct in_addr sin_addr;     // IP 주소
};

// IP 주소 변환
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
```

### 5. 바이트 순서 (Byte Order)

네트워크는 **빅 엔디안(Big Endian)** 을 사용한다. 호스트 바이트 순서와 변환이 필요하다.

```cpp
htons()  // host to network short (포트)
ntohs()  // network to host short
htonl()  // host to network long (IP)
ntohl()  // network to host long
```

### 6. I/O 멀티플렉싱

단일 스레드에서 여러 소켓을 동시에 감시한다.

```cpp
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(sock, &readfds);

select(maxfd + 1, &readfds, nullptr, nullptr, &timeout);

if (FD_ISSET(sock, &readfds)) {
    // 읽기 가능
}
```

---

## 표준 헤더 (Linux/macOS)

```cpp
#include <sys/socket.h>   // socket, bind, listen, accept, connect
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>    // inet_pton, inet_ntop, htons
#include <unistd.h>       // close, read, write
#include <sys/select.h>   // select, fd_set
```

---

## 과제 요약

| 파일 | 내용 |
|------|------|
| `homework.cpp` | TCP 에코 서버 & 클라이언트 (단일 파일, fork 없이 select 사용) |
| `homework2.cpp` | UDP 간단 채팅 (단일 파일, 서버/클라이언트 모드 인자로 선택) |
| `game1.cpp` | 소켓 기반 숫자 맞히기 게임 (서버가 게임 진행, 클라이언트가 추측) |

> 모든 파일은 **단일 실행 파일**로 컴파일되며, 커맨드라인 인자(`--server` / `--client`)로 역할을 선택한다.
