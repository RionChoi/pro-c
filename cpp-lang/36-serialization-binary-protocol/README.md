# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 학습 목표
- 객체를 바이트 스트림으로 변환하는 직렬화(Serialization) 기술 습득
- 이진 프로토콜 설계 및 구현
- 효율적인 데이터 저장 및 네트워크 전송
- 엔디언(Endianness) 처리 및 패킹(Packing)

## 핵심 개념

### 직렬화 (Serialization)
- 메모리 상의 객체를 바이트 스트림으로 변환
- 파일 저장 및 네트워크 전송에 사용
- 텍스트(JSON, XML) vs 이진(Binary) 직렬화

### 이진 프로토콜 (Binary Protocol)
- 구조화된 바이트 형식으로 데이터 정의
- 헤더: 메타정보 (타입, 크기, 버전)
- 페이로드: 실제 데이터
- 고정 크기 vs 가변 크기 필드

### 엔디언 (Endianness)
- 리틀엔디언(Little-Endian): x86, ARM
- 빅엔디언(Big-Endian): 네트워크 바이트 오더
- 변환 함수: `htons()`, `ntohs()` 등

### 직렬화 전략
1. **멤버별 직렬화**: 각 필드를 순서대로 쓰기
2. **버전 관리**: 프로토콜 버전 포함
3. **체크섬/CRC**: 데이터 무결성 검증
4. **압축**: 데이터 크기 최소화

## 과제 설명

### homework.cpp
JSON 직렬화기 구현:
- `SerializerJSON` 클래스: 객체를 JSON 문자열로 변환
- `DeserializerJSON` 클래스: JSON 문자열을 객체로 복원
- `Student` 클래스: 이름, 학번, GPA 필드
- 기본 파싱 및 생성 로직

### homework2.cpp
이진 직렬화기 구현:
- `SerializerBinary` 클래스: 객체를 바이트 스트림으로 변환
- `DeserializerBinary` 클래스: 바이트 스트림을 객체로 복원
- 버전 관리 (버전 1, 버전 2 호환성)
- 체크섬을 이용한 무결성 검증
- 고정 크기 레이아웃 설계

### game1.cpp
**데이터 저장소 게임**:
- 플레이어가 여러 게임 세션 기록 저장/로드
- 이진 포맷으로 저장 및 복원
- 세션별 점수, 타이증, 난이도 기록
- 상위 10개 세션 표시
- 저장 용량 비교: JSON vs Binary

## 컴파일 및 실행

```bash
# 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw36_json
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw36_binary
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o hw36_game

# 실행
./hw36_json
./hw36_binary
./hw36_game
```

## 핵심 포인트
- 직렬화는 데이터 교환의 핵심 기술
- 이진 프로토콜은 텍스트보다 효율적 (크기, 속도)
- 버전 관리와 무결성 검증 필수
- 엔디언 차이를 고려한 이식성(Portability) 확보
