# 세션 48: C++ 그래픽스 & GPU 컴퓨팅

## 학습 목표
1. 그래픽스 파이프라인의 정점 처리, 래스터화, 픽셀 셰이딩 흐름을 이해한다
2. 셰이더 입출력 구조를 컴파일 타임에 검증하는 패턴을 익힌다
3. GPGPU 커널 실행 모델을 CPU 시뮬레이션으로 구현한다
4. 데이터 병렬 처리에서 SoA/연속 메모리 배치가 중요한 이유를 이해한다
5. 외부 그래픽스 SDK 없이 Vulkan 스타일 추상화의 핵심 개념을 학습한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| graphics pipeline | 정점 입력 → 변환 → 래스터화 → 픽셀 색상 출력 단계 |
| shader interface | 셰이더 단계 사이의 입력/출력 구조 계약 |
| push constants | 작은 설정 값을 커맨드마다 빠르게 전달하는 방식 |
| compute kernel | 많은 원소에 같은 연산을 병렬 적용하는 함수 |
| dispatch grid | workgroup과 invocation으로 구성된 실행 범위 |
| buffer view | 연속 메모리를 타입 있는 배열처럼 해석하는 뷰 |

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
