# Session 36: 직렬화 & 이진 프로토콜 (Serialization & Binary Protocols)

## 학습 목표
- 객체를 이진 형식으로 직렬화(Serialization)하는 방법 습득
- 이진 데이터를 역직렬화(Deserialization)하여 객체 복원
- 이진 파일 I/O와 버퍼 관리 이해
- 간단한 이진 프로토콜 설계 및 구현

## 핵심 개념

### 1. 직렬화 (Serialization)
메모리의 객체를 이진 형식으로 변환하여 파일이나 네트워크에 저장할 수 있는 형태로 만드는 과정.

```cpp
struct Point {
    float x, y;
    
    // 직렬화: 객체를 바이트 배열로 변환
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer(sizeof(Point));
        std::memcpy(buffer.data(), this, sizeof(Point));
        return buffer;
    }
    
    // 역직렬화: 바이트 배열을 객체로 복원
    static Point deserialize(const std::vector<uint8_t>& buffer) {
        Point p;
        std::memcpy(&p, buffer.data(), sizeof(Point));
        return p;
    }
};
```

### 2. 이진 파일 I/O
`std::ofstream`, `std::ifstream`의 이진 모드 사용:

```cpp
std::ofstream out("data.bin", std::ios::binary);
out.write(reinterpret_cast<char*>(&data), sizeof(data));
out.close();

std::ifstream in("data.bin", std::ios::binary);
in.read(reinterpret_cast<char*>(&data), sizeof(data));
in.close();
```

### 3. 네트워크 바이트 순서 (Endianness)
- **리틀 엔디안**: x86, ARM (일반적)
- **빅 엔디안**: 네트워크 표준 (Big-Endian)

바이트 순서 변환:
```cpp
#include <cstdint>

uint32_t host_to_network(uint32_t host) {
    return ((host << 24) | ((host << 8) & 0xFF0000) |
            ((host >> 8) & 0xFF00) | (host >> 24));
}
```

### 4. 간단한 이진 프로토콜
구조화된 데이터 전송:
```
[Type: 1 byte] [Length: 4 bytes] [Payload: Length bytes] [Checksum: 1 byte]
```

## 학습 파일

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 기본 직렬화/역직렬화 연습 |
| `homework2.cpp` | 이진 프로토콜 구현 (TCP 패킷 형식) |
| `game1.cpp` | 게임 저장/복구 시스템 (RPG 저장 게임) |

---

## 참고
- C++ 이진 I/O: `<fstream>`, `std::vector<uint8_t>`
- 메모리 레이아웃: `std::memcpy`, `std::memset`
- 바이트 순서: 엔디안 변환 함수
