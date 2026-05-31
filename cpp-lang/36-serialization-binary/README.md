# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocols)

## 학습 목표
- 데이터 구조를 바이너리 형식으로 직렬화(직렬화)하고 역직렬화(역직렬화)하는 방법 습득
- 고정 크기 바이너리 프로토콜 설계 및 구현
- 엔디언(endianness) 처리 및 바이너리 호환성 유지
- 버전 관리를 통한 확장 가능한 프로토콜 설계

## 핵심 개념

### 1. 직렬화 (Serialization)
- **텍스트 vs 바이너리**: JSON은 인간 가독성, 바이너리는 크기 효율성
- **구조체를 바이너리로 변환**: `std::ofstream`과 `std::ifstream` 활용
- **멤버 변수 순서 관리**: 구조체 패딩 고려

### 2. 바이너리 프로토콜
```
[Header] [Body] [Checksum]
  2bytes  N bytes  1byte
```
- **프로토콜 버전**: 하위 호환성 유지
- **길이 필드**: 동적 크기의 데이터 처리
- **체크섬/CRC**: 데이터 무결성 검증

### 3. 엔디언 처리
```cpp
uint32_t value = 0x12345678;
// 리틀 엔디언: 78 56 34 12
// 빅 엔디언: 12 34 56 78
```
- `htons()`, `ntohs()` 네트워크 바이트 순서 변환
- 플랫폼 독립적 직렬화

### 4. 고급 직렬화 패턴
- **타입 이레이저 (Type Erasure)**: `std::any` 대체 직렬 가능
- **템플릿 메타프로그래밍**: 자동 필드 반복 처리
- **비트필드 활용**: 플래그와 작은 정수 팩킹

## 파일 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 간단한 구조체 직렬화/역직렬화 |
| `homework2.cpp` | 바이너리 프로토콜 구현 (헤더, 길이, 체크섬) |
| `game1.cpp` | 저장/로드를 지원하는 게임 상태 관리 시뮬레이터 |

## 컴파일
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw36
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw36_2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game36
```
