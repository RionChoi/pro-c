# Phase 6 진행 계획

작성일: 2026-06-22

진행 시작일: 2026-06-24
현재 진행률: 6/10 세션 (2026-06-30 기준)

## 배경

기존 C/C++ 통합 학습 과정은 `docs/LEARNING_ROADMAP.md` 기준으로 70/70 세션이 완료되었다.
다음 일정은 새 언어 기초가 아니라, 완료한 C++ 역량을 최신 표준과 실무형 엔지니어링 주제로 확장하는 방향이 적절하다.

## Phase 6 목표

- C++26 주요 기능을 실험 가능한 예제로 정리한다.
- 기존 Phase 5의 고성능 C++ 학습을 프로덕션 품질 코드 작성으로 연결한다.
- 표준 라이브러리, 빌드, 테스트, 성능 분석, 안전성 점검을 하나의 실전 흐름으로 묶는다.
- 외부 런타임 의존성은 추가하지 않고, 컴파일러와 표준 라이브러리 지원 범위 안에서 진행한다.

## 권장 일정

| 순서 | 주제 | 산출물 |
|------|------|--------|
| 01 | C++26 표준 현황 점검 | C++26 기능 지원표와 컴파일러 플래그 정리 |
| 02 | Static Reflection 실험 | 타입 메타데이터 출력기와 구조체 필드 순회 예제 |
| 03 | Contracts 기반 방어적 설계 | 사전조건/사후조건을 적용한 API 검증 예제 |
| 04 | `std::execution`와 Senders/Receivers | 비동기 파이프라인 스케치와 fallback 구현 |
| 05 | Safe Reclamation과 동시성 안전성 | hazard pointer/RCU 계열 개념 정리와 샘플 |
| 06 | Modules와 `import std` 재검토 | 기존 세션 일부를 모듈 구조로 재구성 |
| 07 | Sanitizer 기반 품질 점검 | ASan/UBSan/TSan 실행 가이드와 결함 재현 예제 |
| 08 | 벤치마크와 성능 회귀 관리 | 간단한 microbenchmark harness와 결과 기록 양식 |
| 09 | 라이브러리 API 안정화 | 헤더 공개 API, 네임스페이스, 에러 모델 정리 |
| 10 | Phase 6 최종 프로젝트 | C++26 기능을 활용한 타입 안전 설정/직렬화 미니 프레임워크 |

## 진행 방식

1. `docs/LEARNING_ROADMAP.md`의 Phase 6 섹션을 공식 진행 기준으로 사용한다.
2. 각 세션은 기존 패턴처럼 `README.md`, `homework.cpp`, `homework2.cpp`, `game1.cpp`를 둔다.
3. C++26 기능은 컴파일러 지원 여부에 따라 `#if` 또는 fallback 설명을 함께 둔다.
4. 실험 기능은 “표준 채택 여부”와 “현재 컴파일러 지원 여부”를 분리해서 기록한다.

## 참고 자료

- WG21 C++26 working draft N5032: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/n5032.pdf
- WG21 meeting schedule: https://isocpp.org/std/meetings-and-participation/upcoming-meetings
- Reflection for C++26 proposal P2996R13: https://isocpp.org/files/papers/P2996R13.html
- C++26 compiler support table: https://en.cppreference.com/w/cpp/compiler_support/26
- C++26 Senders/Receivers overview: https://isocpp.org/blog/2025/01/senders-receivers-an-introduction-lucian-radu-teodorescu
