# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 학습 목표
- 객체를 바이너리 형식으로 직렬화(Serialization)하는 방법 이해
- 바이너리 데이터를 객체로 역직렬화(Deserialization)하는 방법 학습
- 네트워크 통신이나 파일 저장을 위한 이진 프로토콜 설계
- 엔디안(Endianness), 정렬(Padding), 버전 관리(Version Control) 고려

## 핵심 개념

### 1. 직렬화의 필요성
- 메모리 상의 데이터를 전송 또는 저장 가능한 형식으로 변환
- 네트워크 통신, 파일 I/O, 프로세스 간 통신에 필수
- 텍스트 기반(JSON, XML) vs 바이너리(Protocol Buffers, 커스텀) 방식

### 2. 바이너리 직렬화 기법
- `std::ofstream`/`std::ifstream`의 이진 모드 사용
- `reinterpret_cast`와 포인터를 이용한 메모리 레이아웃 접근
- 엔디안 변환의 필요성 (big-endian vs little-endian)

### 3. 구조체와 메모리 레이아웃
- `#pragma pack()` 또는 `__attribute__((packed))`로 정렬 제어
- 패딩(Padding)이 성능과 직렬화 결과에 미치는 영향
- 이식성(Portability)을 위한 고려사항

### 4. 프로토콜 설계
- 헤더(Magic Number, Version, Length) 구성
- 메시지 타입(Message Type) 정의
- 체크섬(Checksum) 또는 CRC를 이용한 무결성 검증

### 5. 버전 관리 및 확장성
- 스키마 진화(Schema Evolution) 시뮬레이션
- 이전 버전과의 호환성 유지
- 필드 추가 및 제거 시나리오 처리

## 예제 시나리오

### homework.cpp
기본 직렬화 구현:
- 간단한 `Person` 구조체 정의
- 바이너리 파일에 쓰기/읽기
- 바이트 순서 변환(호스트 ↔ 네트워크)

### homework2.cpp
고급 직렬화 (버전 관리 포함):
- 버전 필드가 있는 메시지 포맷
- 여러 버전의 데이터 읽기
- 체크섬 검증

### game1.cpp
게임 응용:
- RPG 캐릭터 세이브/로드 게임
- 여러 캐릭터 관리 및 통신
- 인벤토리와 경험치 저장

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

## 학습 핵심 포인트
1. **메모리 접근**: 포인터와 `reinterpret_cast`의 안전한 사용
2. **이식성**: 엔디안과 패딩을 고려한 이진 포맷 설계
3. **신뢰성**: 체크섬/헤더를 통한 데이터 무결성 검증
4. **확장성**: 버전 번호를 통한 프로토콜 진화 관리

## 참고 자료
- C++17 Filesystem, fstream 공식 문서
- Protocol Buffers, MessagePack 설계 참고
- IEEE 754 부동소수점 표준
