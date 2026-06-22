# 세션 49: C++ 네트워크 & Proactor 패턴

## 학습 목표
1. Reactor와 Proactor의 차이를 이해한다
2. 완료 이벤트 큐 기반의 Proactor 실행 모델을 구현한다
3. HTTP/2 프레임 헤더와 payload 직렬화/역직렬화를 연습한다
4. 코루틴 기반 비동기 I/O 흐름을 직접 설계한다
5. 외부 Asio 없이 표준 라이브러리로 이벤트 루프와 completion handler 패턴을 학습한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| Reactor | 준비된 I/O 이벤트를 감지하고 사용자가 직접 I/O 수행 |
| Proactor | I/O 작업 요청 후 완료 이벤트를 받아 후속 처리 |
| completion handler | 비동기 작업 완료 시 호출되는 콜백 |
| HTTP/2 frame | 9바이트 헤더와 payload로 구성된 이진 메시지 단위 |
| coroutine I/O | `co_await`로 비동기 작업 완료 지점을 표현 |
| event loop | 완료 큐를 순회하며 콜백과 코루틴을 재개하는 실행기 |

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
