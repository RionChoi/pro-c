# pro-c 학습 로드맵 (기초 → 실전)

## 목표
- C 문법 기초를 정확히 익힌다.
- 입력/출력, 조건/반복, 함수, 배열/문자열, 포인터를 단계적으로 습득한다.
- 작은 콘솔 프로젝트를 스스로 설계/구현/검증한다.

## 현재 진도 (2026-04-10 기준)
- 완료된 강의 노트:
  - `course/01-variables-io/README.md`
  - `course/02-condition-loop/README.md`
  - `course/03-function-array/README.md`
  - `course/04-pointers-strings/README.md`
- 생성된 과제/게임 파일:
  - 각 강의별 `homework.c`, `homework2.c`, `game1.c` 생성 완료
- 코드 리뷰 최신화:
  - `CODE_REVIEW.md` 업데이트 완료
- 현재 코드 상태:
  - 대상: `Helloworl.c`
  - 컴파일 경고 1건(`main` 프로토타입)
  - 입력 검증(`scanf` 반환값 확인) 보완 필요

---

## 주차별 학습 계획 (업데이트)

### 1주차 — C 기본기 (진행 중)
- 자료형, 연산자, printf/scanf, 조건문, 반복문
- 과제: lesson1~2 숙제 완성 및 제출
- 체크포인트:
  - [ ] `scanf` 반환값 검증 습관화
  - [ ] `int main(void)` 표준 형태 고정

### 2주차 — 함수/배열/문자열 (진행 중)
- 함수 분리, 배열 통계, 문자열 입력/처리
- 과제: lesson3 숙제 + 문자열 처리 미니 실습
- 체크포인트:
  - [ ] 함수 단위로 역할 분리
  - [ ] 배열 범위 검사

### 3주차 — 포인터/메모리 (시작)
- 포인터 기본, 배열-포인터 관계, 동적 메모리 입문
- 오늘 시작 자료: `lesson-04-pointers-strings.md`, `lesson4_homework.c`
- 체크포인트:
  - [ ] `%p` 출력 + `(void *)` 캐스팅 숙지
  - [ ] 포인터 역참조 전 유효성 확인

### 4주차 — 파일 I/O + 미니 프로젝트
- `fopen/fgets/fprintf/fclose`
- 프로젝트 후보: 성적 관리 CLI / 할일 관리 CLI
- 체크포인트:
  - [ ] 에러 처리(파일 열기 실패, 입력 실패)
  - [ ] 모듈화(함수별 파일 분리)

---

## 다음 리뷰 전 권장 액션
1. `Helloworl.c`를 `hello_world.c`로 리네임
2. `main()` → `main(void)` 수정
3. `scanf` 실패 처리 코드 추가
4. lesson4 과제(`course/04-pointers-strings/homework.c`)를 직접 확장:
   - 최소값 함수 추가
   - 정렬(오름차순) 기능 추가
   - 입력 재시도 로직(최대 3회) 추가

## C 학습 완료 후 다음 단계
- C 기본/중급(포인터, 메모리, 파일 I/O, 미니 프로젝트) 완료 시
- 동일 흐름으로 C++ 과정으로 전환:
  1) C++ 기초 문법(`iostream`, 클래스, STL)
  2) 객체지향(캡슐화/상속/다형성)
  3) 자료구조/알고리즘 실전
  4) C 프로젝트를 C++로 리팩터링
