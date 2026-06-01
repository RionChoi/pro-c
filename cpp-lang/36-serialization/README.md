# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocol)

## 학습 목표
1. **데이터 직렬화**: 메모리 구조를 바이트 스트림으로 변환
2. **이진 프로토콜**: 바이트 단위로 데이터를 읽고 쓰는 방법
3. **타입 안전성**: 가변 길이 데이터의 효율적인 처리
4. **파일 기반 저장**: 구조체를 파일에 저장하고 복구

## 핵심 개념

### 직렬화 (Serialization)
- **직렬화**: 객체 상태를 바이트 스트림으로 변환
- **역직렬화**: 바이트 스트림을 객체로 복원
- **용도**: 파일 저장, 네트워크 전송, 캐싱

### 이진 프로토콜 설계
- **고정 크기 필드**: 정수, 부동소수점, 고정 크기 배열
- **가변 길이 필드**: 문자열, 동적 배열
- **바이트 순서 (Endianness)**: 빅엔디언 vs 리틀엔디언

### 메모리 레이아웃
```cpp
struct Record {
    uint16_t id;          // 2바이트
    char name[64];        // 64바이트
    double value;         // 8바이트
    uint32_t flag;        // 4바이트
};
// 총 78바이트 (패딩 무시)
```

### 바이너리 스트림 I/O
```cpp
#include <fstream>
std::ofstream out("data.bin", std::ios::binary);
Record r = {123, "Alice", 3.14, 1};
out.write(reinterpret_cast<char*>(&r), sizeof(r));
out.close();
```

## 과제 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 구조체 직렬화: Student 레코드 저장/로드 |
| `homework2.cpp` | 확장 프로토콜: 가변 길이 문자열 직렬화 |
| `game1.cpp` | 미니 게임: 점수 저장 및 랭킹 시스템 |

## 컴파일 & 실행

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw
./hw

g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
./hw2

g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game
./game
```

## 주의사항
- 포인터는 직렬화할 수 없음 (주소는 런타임 의존적)
- 패딩과 align 문제 고려
- 다중 플랫폼 호환성을 위해 바이트 순서 명시
- 파일 I/O 오류 처리 필수
