# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 학습 목표
- 객체를 바이트 형식으로 변환하는 직렬화(Serialization) 이해
- JSON 기반 직렬화 구현 (텍스트 형식)
- 바이너리 프로토콜 설계 및 구현
- 네트워크 통신이나 파일 저장을 위한 데이터 인코딩

## 핵심 개념

### 1. 직렬화란?
메모리의 객체 상태를 바이트 스트림으로 변환하는 과정
- **텍스트 직렬화**: JSON, CSV, XML
- **이진 직렬화**: 바이너리 프로토콜, Protocol Buffers, MessagePack

### 2. JSON 기반 직렬화
```cpp
class Student {
    int id;
    std::string name;
    double gpa;
public:
    std::string toJson() const;
    static Student fromJson(const std::string& json);
};
```

### 3. 바이너리 프로토콜
효율적인 데이터 저장 및 전송
- 타입 정보 인코딩
- 길이 필드 사용
- 엔디안 고려

### 4. 직렬화 방식
| 방식 | 크기 | 속도 | 가독성 | 용도 |
|------|------|------|--------|------|
| JSON | 큼 | 느림 | 높음 | 설정, API |
| 이진 | 작음 | 빠름 | 낮음 | 네트워크, 저장소 |

## 과제 설명

### `homework.cpp`
**목표**: JSON 직렬화 구현
- `Student` 클래스: id, name, gpa
- `toJson()`: 객체를 JSON 문자열로 변환
- `fromJson(str)`: JSON에서 객체 복원
- 파일에 JSON 저장/읽기

### `homework2.cpp`
**목표**: 바이너리 프로토콜 직렬화
- 간단한 이진 형식 설계
- `toBinary()`: 객체를 바이트 배열로 변환
- `fromBinary(data)`: 바이트에서 객체 복원
- 타입 정보와 길이 필드 포함

### `game1.cpp`
**목표**: 직렬화를 활용한 게임 (데이터 저장/로드)
- 플레이어 정보를 파일에 저장/로드
- 게임 데이터 자동 저장 기능
- 저장된 데이터 검증

## 참고
- C++17 `std::stringstream` 활용
- 파일 I/O: `<fstream>`
- 간단한 JSON 파싱 (외부 라이브러리 없이)
