# C++-15: 파일 I/O & 스트림 (File I/O & Streams)

## 학습 목표
1. C++ 스트림 기반 파일 I/O (`ifstream`, `ofstream`, `fstream`)
2. 텍스트 파일과 이진 파일 처리
3. 스트림 상태 확인 (`good()`, `eof()`, `fail()`, `bad()`)
4. 파일 위치 지정자 (`seekg()`, `seekp()`, `tellg()`, `tellp()`)
5. 포맷된 입출력 및 조작자 (manipulators)

## 핵심 개념

### ifstream (입력 파일 스트림)
```cpp
std::ifstream file("input.txt");
if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
        // 파일 읽기
    }
    file.close();
}
```

### ofstream (출력 파일 스트림)
```cpp
std::ofstream file("output.txt");
if (file.is_open()) {
    file << "데이터 쓰기" << std::endl;
    file.close();
}
```

### 스트림 상태
- `good()` — 모든 상태 좋음
- `eof()` — 파일 끝 도달
- `fail()` — 입출력 오류
- `bad()` — 심각한 오류

### 파일 위치 지정
```cpp
file.seekg(10);              // 10바이트로 이동
std::streampos pos = file.tellg();  // 현재 위치 확인
```

## 과제
- **homework.cpp**: 텍스트 파일 읽고 쓰기, 줄 단위 처리
- **homework2.cpp**: 이진 파일 작업, 구조체 저장/로드
- **game1.cpp**: 간단한 점수 저장/로드 시스템 구현 게임
