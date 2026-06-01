# Session 36: 직렬화 & 이진 프로토콜 (Serialization)

## 학습 목표
1. **직렬화 개념** — 객체를 바이트 스트림으로 변환
2. **이진 프로토콜** — 효율적인 데이터 교환
3. **직렬화 패턴** — Visitor, Template 기반 접근
4. **역직렬화** — 바이트 스트림을 객체로 복원

## 핵심 개념

### 직렬화(Serialization)란?
- 메모리의 객체를 파일이나 네트워크로 전송 가능한 형태로 변환
- **텍스트 기반**: JSON, CSV, XML
- **이진 기반**: protobuf, MessagePack, 커스텀 이진 포맷

### 이진 프로토콜의 장점
- 파일 크기 최소화
- 파싱 속도 향상
- 타입 안정성

### 직렬화 구현 방법
1. **Visitor 패턴**: 객체 구조를 순회하며 직렬화
2. **Template 기반**: `serialize<T>()` 함수 오버로드
3. **Virtual 함수**: 각 클래스가 직렬화 메서드 구현

## 학습 구성

| 파일 | 주제 |
|------|------|
| `homework.cpp` | Visitor 패턴 기반 객체 직렬화 (JSON) |
| `homework2.cpp` | 이진 프로토콜 직렬화 (구조체, 배열) |
| `game1.cpp` | 직렬화 캐릭터 관리 게임 (저장/로드) |

---

## 구현 가이드

### homework.cpp: Visitor 패턴 직렬화
**요구사항**:
- `Person` 클래스 (이름, 나이)
- `Student` 클래스 (Person 상속, 학번, 성적배열)
- `Serializer` 클래스 (Visitor 패턴)
- JSON 형식의 `to_json()` 함수
- 역직렬화 `from_json()` 함수

**예시 출력**:
```json
{
  "type": "Student",
  "name": "Alice",
  "age": 20,
  "student_id": "S001",
  "scores": [85, 90, 78]
}
```

---

### homework2.cpp: 이진 프로토콜 직렬화
**요구사항**:
- 구조체 기반 이진 포맷 정의
- `BinaryWriter` 클래스 (쓰기)
- `BinaryReader` 클래스 (읽기)
- 정수, 문자열, 배열 직렬화
- 파일 저장/로드

**이진 포맷**:
```
[type_id: 1 byte] [length: 4 bytes] [data: variable]
```

---

### game1.cpp: 캐릭터 관리 게임
**요구사항**:
- `Character` 클래스 (이름, 체력, 경험치, 인벤토리)
- `Item` 클래스 (이름, 효과)
- JSON으로 캐릭터 저장
- JSON에서 캐릭터 로드
- 게임 메뉴:
  1. 새 캐릭터 생성
  2. 저장된 캐릭터 로드
  3. 캐릭터 정보 출력
  4. 게임 진행 (체력 감소, 인벤토리 추가)
  5. 캐릭터 저장 및 종료

---

## 학습 포인트
- ✅ 오버로드를 통한 템플릿 기반 직렬화
- ✅ Visitor 패턴의 실제 응용
- ✅ JSON과 이진 포맷의 차이
- ✅ 파일 I/O와 직렬화의 결합
- ✅ 에러 처리 (잘못된 포맷, 손상된 파일)

