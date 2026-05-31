# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocols)

## 학습 목표
- 객체를 바이너리 형식으로 직렬화(serialize)하는 방법 이해
- 파일 및 네트워크 통신을 위한 데이터 인코딩/디코딩
- 다양한 직렬화 기법 비교: 바이너리, JSON, Protocol Buffers 개념
- 엔디언(Endian) 및 바이너리 패킹(binary packing) 이해

## 핵심 개념

### 1. 직렬화 (Serialization)
메모리의 객체를 파일이나 네트워크를 통해 전송 가능한 형식으로 변환.
- 인코딩(Encoding): 객체 → 바이너리
- 디코딩(Decoding): 바이너리 → 객체

### 2. 바이너리 직렬화
고정 크기 바이너리 형식으로 빠르고 컴팩한 저장.

### 3. JSON 직렬화
텍스트 기반, 사람이 읽기 쉬운 형식 (수동 구현).

### 4. 엔디언
- **Little Endian**: 최하위 바이트가 먼저 (Intel, x86)
- **Big Endian**: 최상위 바이트가 먼저 (네트워크 표준)

## 파일 구성
- `homework.cpp` — 기본 바이너리 직렬화
- `homework2.cpp` — JSON 직렬화 (수동 구현)
- `game1.cpp` — 게임 저장/로드 시스템

## 사전 개념
- `std::ofstream`, `std::ifstream` (파일 I/O)
- `std::stringstream` (메모리 스트림)
- `memcpy()`, `reinterpret_cast`
- `std::vector`
