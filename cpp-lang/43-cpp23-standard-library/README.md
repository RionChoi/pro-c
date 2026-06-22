# 세션 43: C++23 표준 라이브러리 신기능

## 학습 목표
1. `std::expected` 스타일의 명시적 성공/실패 반환 모델을 익힌다
2. `std::flat_map`의 정렬된 연속 메모리 기반 맵 개념을 이해한다
3. `std::mdspan`의 다차원 뷰 모델을 학습한다
4. `std::print` 스타일의 출력 API가 기존 iostream 출력과 어떻게 다른지 비교한다
5. 표준 라이브러리 지원 여부가 다른 환경에서도 컴파일되는 fallback 설계를 연습한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| `std::expected<T,E>` | 예외 대신 값 또는 에러를 명시적으로 반환하는 타입 |
| flat map | 정렬된 배열/vector 위에 map 인터페이스를 얹은 자료구조 |
| mdspan | 소유권 없이 다차원 배열처럼 접근하는 view 타입 |
| print API | 포맷 문자열 기반의 간결한 출력 인터페이스 |
| feature-test macro | 표준 기능 지원 여부를 컴파일 타임에 확인하는 매크로 |

## 구현 메모

일부 C++23 라이브러리 기능은 컴파일러/표준 라이브러리 버전에 따라 아직 제공되지 않을 수 있습니다. 이 세션의 예제는 지원되는 기능은 표준 타입을 사용하고, 미지원 기능은 동일한 학습 목적의 작은 fallback 타입으로 대체합니다.

## 컴파일 명령어

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
