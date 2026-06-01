# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocols)

## 학습 목표
- 객체를 바이너리 형태로 변환하여 저장 및 전송
- 텍스트 기반 직렬화 (JSON-like 포맷)
- 바이너리 직렬화의 효율성 이해
- 버전 관리 및 확장성 고려
- 파일 I/O와 직렬화의 결합

## 핵심 개념

### 1. 직렬화란?
- 메모리의 객체 상태를 저장 가능한 형식으로 변환
- 역직렬화: 저장된 형식을 다시 객체로 복원

### 2. 직렬화 방식
- **텍스트 직렬화**: JSON, CSV, XML
- **바이너리 직렬화**: 고정 포맷, 효율적

### 3. 설계 고려사항
- 버전 호환성
- 엔디안(Endianness) 처리
- 동적 크기 데이터 (벡터, 문자열)

## 파일 구성

| 파일 | 설명 |
|------|------|
| `homework.cpp` | 간단한 구조체의 바이너리 직렬화/역직렬화 |
| `homework2.cpp` | 복잡한 객체 직렬화 (벡터, 동적 메모리) |
| `game1.cpp` | 게임 세이브/로드 시스템 |

## 컴파일 및 실행

```bash
# homework.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw36
./hw36

# homework2.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw36_2
./hw36_2

# game1.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game36
./game36
```

## 학습 포인트
- 이진 파일 I/O 패턴
- 구조체 패킹과 정렬
- 문자열과 동적 배열의 직렬화
- 저장/복원의 대칭성 보장
- 오류 처리와 데이터 검증
