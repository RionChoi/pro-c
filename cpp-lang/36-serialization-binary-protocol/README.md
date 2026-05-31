# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 학습 목표
- 객체를 이진 형식으로 직렬화/역직렬화하기
- 커스텀 이진 프로토콜 설계 및 구현
- 버전 관리와 호환성 처리
- 파일 포맷 헤더 및 메타데이터 관리

## 핵심 개념

### 1. 이진 직렬화의 개요
- **텍스트 vs 이진**: 이진은 더 효율적이지만 바이트 순서(endianness)와 정렬(alignment) 고려 필요
- **구조화된 포맷**: 매직 넘버(magic number), 버전 정보, 체크섬

### 2. 커스텀 프로토콜
```cpp
// 간단한 포맷:
// [매직넘버 4B][버전 2B][데이터길이 4B][데이터][체크섬 4B]
```

### 3. 직렬화 구현 패턴
- 스트림 기반: std::ostream, std::istream 활용
- 버퍼 기반: std::vector<char> 직접 관리

### 4. 에러 처리
- 파일 접근 오류
- 손상된 데이터 감지
- 버전 호환성 확인

## 과제 설명

### homework.cpp: 기본 이진 직렬화
- 간단한 구조체(이름, 나이, 점수)를 이진 형식으로 저장/로드
- 매직 넘버와 버전 정보 포함
- 체크섬(CRC32) 검증

### homework2.cpp: 복잡한 데이터 구조 직렬화
- 배열이나 벡터 포함하는 구조체 직렬화
- 중첩된 객체 처리
- 동적 크기 데이터 처리

### game1.cpp: 게임 저장/로드 시스템
- 간단한 RPG 게임 상태를 직렬화
- "Save Game" / "Load Game" 기능
- 게임 진행 상태(캐릭터, 인벤토리, 레벨)를 파일로 저장

## 컴파일 및 실행

```bash
# homework 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/36-serialization-binary-protocol/homework.cpp -o hw36
./hw36

# homework2 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/36-serialization-binary-protocol/homework2.cpp -o hw36_2
./hw36_2

# game1 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/36-serialization-binary-protocol/game1.cpp -o game36
./game36
```

## 참고 자료
- std::fstream, std::ofstream, std::ifstream
- std::vector, std::string
- 바이트 순서 변환 (네트워크 바이트 오더)
- CRC32 체크섬 계산
