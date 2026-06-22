# 세션 13: STL 알고리즘 (STL Algorithms)

## 학습 목표
- STL에서 제공하는 주요 알고리즘 함수들을 이해한다.
- `<algorithm>` 헤더의 std::sort, std::find, std::transform 등을 활용한다.
- 람다 함수와 함께 STL 알고리즘을 사용하는 방법을 배운다.
- 범위 기반 연산과 함수형 프로그래밍 패러다임을 이해한다.

## 핵심 개념
1. **정렬 (Sorting)**
   - `std::sort()` — 배열/벡터 정렬
   - `std::stable_sort()` — 안정 정렬
   - `std::partial_sort()` — 부분 정렬

2. **탐색 (Searching)**
   - `std::find()` — 선형 탐색
   - `std::binary_search()` — 이진 탐색
   - `std::lower_bound()`, `std::upper_bound()` — 경계값 탐색

3. **변환 (Transformation)**
   - `std::transform()` — 각 원소 변환
   - `std::copy()`, `std::copy_if()` — 조건부 복사
   - `std::fill()`, `std::generate()` — 값 채우기

4. **집계 (Aggregation)**
   - `std::accumulate()` — 합계 계산
   - `std::count()`, `std::count_if()` — 개수 세기
   - `std::any_of()`, `std::all_of()`, `std::none_of()` — 조건 검사

5. **람다와의 결합**
   - 커스텀 비교 함수 작성
   - 조건부 필터링
   - 함수형 프로그래밍

## 파일 설명
- **homework.cpp**: STL 정렬 및 탐색 알고리즘 기초
- **homework2.cpp**: 고급 변환 및 집계 알고리즘
- **game1.cpp**: 점수 관리 시스템 (std::sort, std::transform 활용)

## 참조 자료
- Bro Code C++ 강좌: STL 알고리즘 섹션
- cppreference.com: `<algorithm>` 헤더 문서
