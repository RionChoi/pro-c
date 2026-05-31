# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 목표
- C++ 객체를 이진 프로토콜로 직렬화/역직렬화하기
- 파일 I/O를 활용한 데이터 저장 및 복원
- 프로토콜 버전 관리 및 확장성 고려

## 학습 내용

### 1. 이진 직렬화 (Binary Serialization)
- `ostream::write()`, `istream::read()` 사용
- 고정 크기 데이터 저장
- 바이트 순서(endianness) 고려

### 2. 문자열 직렬화
- 길이 정보 함께 저장
- 메모리 안전성 보장

### 3. 프로토콜 설계
- 매직 넘버로 파일 검증
- 버전 정보 포함
- 체크섬을 통한 무결성 검사 (선택)

### 4. STL 컨테이너 직렬화
- `std::vector` 직렬화
- `std::map` 직렬화

## 과제

### homework.cpp
JSON 스타일의 텍스트 포맷으로 Student 구조체 배열 직렬화/역직렬화

### homework2.cpp
바이너리 프로토콜 구현: 
- 매직 넘버(0xCAFE)
- 버전(1)
- Student 레코드 배열 저장

### game1.cpp
**데이터 백업/복원 게임**:
사용자가 여러 항목(아이템, 설정 등)을 입력 → 파일로 저장 → 로드하여 검증
