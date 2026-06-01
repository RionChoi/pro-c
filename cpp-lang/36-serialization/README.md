# Session 36: 직렬화 & 이진 프로토콜 (Serialization)

## 학습 목표
- 객체를 바이트 스트림으로 변환하고 역변환하는 직렬화 기법 습득
- 이진 프로토콜 설계 및 구현
- 파일 및 네트워크 통신에서 데이터 영속성 구현

## 핵심 개념

### 직렬화 (Serialization)
- 객체를 바이트 열로 변환
- 파일 저장, 네트워크 전송, 메모리 스냅샷 등에 사용
- 텍스트 직렬화 (JSON, XML) vs 이진 직렬화

### 이진 프로토콜
- 데이터 포맷 정의 (구조, 엔디안, 패딩)
- 버전 관리 및 호환성
- 압축, 암호화와의 조합

### C++에서의 구현
```cpp
// 기본 패턴: 이진 파일 I/O
struct Data {
    int id;
    double value;
    std::string name;
    
    // 직렬화: 객체 → 바이트
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buf;
        // id 저장 (4바이트)
        buf.push_back((id >> 0) & 0xFF);
        buf.push_back((id >> 8) & 0xFF);
        buf.push_back((id >> 16) & 0xFF);
        buf.push_back((id >> 24) & 0xFF);
        // value 저장 (8바이트 double)
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        buf.insert(buf.end(), ptr, ptr + sizeof(double));
        // name 저장 (길이 + 문자열)
        uint32_t len = name.size();
        buf.push_back((len >> 0) & 0xFF);
        buf.push_back((len >> 8) & 0xFF);
        buf.insert(buf.end(), name.begin(), name.end());
        return buf;
    }
    
    // 역직렬화: 바이트 → 객체
    static Data deserialize(const std::vector<uint8_t>& buf, size_t& pos) {
        Data d;
        d.id = buf[pos] | (buf[pos+1] << 8) | (buf[pos+2] << 16) | (buf[pos+3] << 24);
        pos += 4;
        d.value = *reinterpret_cast<const double*>(&buf[pos]);
        pos += 8;
        uint32_t len = buf[pos] | (buf[pos+1] << 8);
        pos += 2;
        d.name = std::string(buf.begin() + pos, buf.begin() + pos + len);
        pos += len;
        return d;
    }
};
```

### 엔디안 처리
```cpp
// 네트워크 바이트 순서 (빅 엔디안) 변환
uint32_t to_network_order(uint32_t host_order) {
    return htonl(host_order); // host to network long
}

uint32_t to_host_order(uint32_t network_order) {
    return ntohl(network_order); // network to host long
}
```

### 스트림 기반 직렬화
```cpp
// 이진 파일 쓰기
std::ofstream out("data.bin", std::ios::binary);
Data d{42, 3.14, "hello"};
std::vector<uint8_t> buf = d.serialize();
out.write(reinterpret_cast<const char*>(buf.data()), buf.size());

// 이진 파일 읽기
std::ifstream in("data.bin", std::ios::binary);
std::vector<uint8_t> buf((std::istreambuf_iterator<char>(in)), 
                         std::istreambuf_iterator<char>());
size_t pos = 0;
Data d = Data::deserialize(buf, pos);
```

## 할당 (Homework)

### homework.cpp — 직렬화 구현
간단한 `Person` 구조체 (이름, 나이, 점수)를 정의하고:
1. 이진 형식으로 직렬화하는 메서드 구현
2. 직렬화된 바이트를 파일에 저장
3. 파일에서 읽어 역직렬화하여 원본과 비교

**요구사항**:
- `std::vector<uint8_t>` 사용
- 엔디안 안정성 확인
- 에러 처리 포함

### homework2.cpp — 프로토콜 버전 관리
여러 버전의 데이터 포맷을 지원하는 직렬화 구현:
1. 버전 헤더 추가 (버전 번호 기록)
2. 버전 1: 기본 필드
3. 버전 2: 추가 필드 (새로운 정보)
4. 역직렬화 시 버전에 따라 다르게 처리

### game1.cpp — 직렬화 게임: 세이브/로드 시뮬레이션
간단한 RPG 데이터 관리 시뮬레이터:
1. 캐릭터 정보 (이름, 레벨, HP, 인벤토리)
2. 캐릭터 상태 변경 (레벨업, 아이템 습득)
3. 게임 세이브: 모든 캐릭터를 바이너리 파일로 저장
4. 게임 로드: 파일에서 모든 캐릭터 복원
5. 세이브/로드 검증: 원본과 로드된 데이터가 일치하는지 확인

**요구사항**:
- 최소 2개 캐릭터 관리
- 인벤토리는 아이템 리스트로 구성
- 세이브/로드 성공 여부 출력

## 참고 자료
- `<fstream>`, `<vector>`, `<cstring>` (memcpy 등)
- 바이트 단위 읽기/쓰기
- 네트워크 바이트 순서 (htonl, ntohl)
