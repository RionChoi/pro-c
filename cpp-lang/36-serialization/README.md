# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 학습 목표
- 객체를 바이트 스트림으로 변환하는 직렬화(Serialization) 이해
- 메모리 레이아웃과 바이트 순서(Endianness) 고려
- JSON 직렬화와 이진 프로토콜의 차이점 학습
- 효율적인 데이터 전송과 저장 구현

## 핵심 개념

### 1. 직렬화 (Serialization)
- 객체를 저장 또는 전송 가능한 형식(바이트 스트림)으로 변환
- JSON(텍스트) vs Binary(이진) 직렬화
- 역직렬화: 바이트 스트림을 원래 객체로 복원

### 2. 이진 프로토콜 설계
- 헤더(magic number, version, size)
- 필드 구성(타입, 크기, 순서)
- 패딩 고려사항
- 엔디안 처리(Big-Endian vs Little-Endian)

### 3. 방법론
- 직접 구현: `memcpy()`, 수동 바이트 변환
- 메타데이터 포함: 필드 개수, 타입 정보
- 에러 처리: CRC/체크섬 검증

## 세션 파일
- `homework.cpp` — 구조체의 이진 직렬화/역직렬화
- `homework2.cpp` — 동적 배열을 포함한 복잡한 직렬화
- `game1.cpp` — 게임 저장/로드 시스템 구현
