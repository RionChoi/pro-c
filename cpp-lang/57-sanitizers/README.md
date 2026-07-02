# 세션 57: Sanitizer 기반 품질 점검

## 학습 목표
1. AddressSanitizer, UndefinedBehaviorSanitizer, ThreadSanitizer의 역할을 구분한다
2. 기본 실행은 안전하게 유지하고, 의도적인 결함 재현 경로만 따로 연다
3. 같은 프로그램을 평소 빌드와 sanitizer 빌드로 나눠 보는 습관을 익힌다
4. ASan/UBSan은 조합 가능하지만 TSan은 보통 별도 빌드가 필요함을 이해한다
5. 결함을 찾는 도구와 결함을 고치는 코드 경로를 분리해서 설명할 수 있다

## 2026-07-01 기준 상태

- 로컬 Apple Clang 21 환경에서 `-fsanitize=address`, `-fsanitize=undefined`,
  `-fsanitize=thread` 빌드가 모두 동작했다.
- TSan은 간단한 데이터 레이스 샘플에서 실제 경고를 출력했다.
- 따라서 이번 세션은 "항상 안전하게 실행되는 기본 경로"와
  "`--demo-*` 인자로만 결함을 터뜨리는 재현 경로"를 함께 둔다.

## 핵심 개념

| 도구 | 잡는 문제 | 이 세션 예제 |
|------|-----------|--------------|
| ASan | 힙/스택/전역 out-of-bounds, use-after-free, 일부 leak | `homework.cpp`의 `--demo-overflow`, `--demo-uaf` |
| UBSan | signed overflow, bad shift, misaligned access 등 UB | `homework2.cpp`의 `--demo-overflow`, `--demo-shift` |
| TSan | 데이터 레이스 | `game1.cpp`의 `--demo-race` |

## 파일 구성

### `homework.cpp`
**AddressSanitizer 재현기**
- 기본 경로는 `std::vector::at()`과 `std::unique_ptr`로 안전하게 처리
- `--demo-overflow`는 heap out-of-bounds
- `--demo-uaf`는 use-after-free

### `homework2.cpp`
**UndefinedBehaviorSanitizer 재현기**
- 기본 경로는 overflow와 shift를 사전 검증
- `--demo-overflow`는 signed integer overflow
- `--demo-shift`는 음수 시프트

### `game1.cpp`
**ThreadSanitizer 재현기**
- 기본 경로는 mutex로 점수판 보호
- `--demo-race`는 고의로 잠금 없이 같은 정수를 갱신
- 같은 시뮬레이터에서 안전/위험 경로를 비교

## 기본 컴파일 명령어

```bash
clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion game1.cpp -o game
```

## Sanitizer 재현 명령어

Clang 문서 권장에 맞춰 `-g`, `-fno-omit-frame-pointer`를 붙인다.

```bash
# ASan
clang++ -std=c++20 -g -fno-omit-frame-pointer -fsanitize=address \
  homework.cpp -o hw_asan
./hw_asan --demo-overflow
./hw_asan --demo-uaf

# UBSan
clang++ -std=c++20 -g -fno-omit-frame-pointer -fsanitize=undefined \
  homework2.cpp -o hw2_ubsan
./hw2_ubsan --demo-overflow
./hw2_ubsan --demo-shift

# TSan
clang++ -std=c++20 -g -fno-omit-frame-pointer -fsanitize=thread \
  game1.cpp -o game_tsan
./game_tsan --demo-race
```

ASan과 UBSan은 함께 묶을 수 있지만, TSan은 별도 런타임을 쓰므로 보통 따로 빌드한다.

## 참고 자료

- [Clang AddressSanitizer 문서](https://clang.llvm.org/docs/AddressSanitizer.html)
- [Clang UndefinedBehaviorSanitizer 문서](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- [Clang ThreadSanitizer 문서](https://clang.llvm.org/docs/ThreadSanitizer.html)
