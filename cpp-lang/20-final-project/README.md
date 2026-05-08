# C++-20: C++ 최종 프로젝트 — 도서관 관리 시스템

## 개요
C++ 1~19세션에서 학습한 모든 개념을 종합하여 **텍스트 기반 도서관 관리 시스템**을 구현합니다.

## 적용된 핵심 개념

| 세션 참조 | 개념 | 적용 위치 |
|-----------|------|-----------|
| 07 (OOP 클래스) | Book, Member, Library 클래스 | homework.cpp |
| 08 (생성자/소멸자) | RAII, 초기화 리스트 | homework.cpp |
| 09 (상속/다형성) | Item 추상 클래스 → Book, Magazine | homework.cpp |
| 10 (연산자 오버로딩) | `<<` 출력 연산자 | homework.cpp |
| 11 (템플릿) | `search<T>` 범용 검색 함수 | homework2.cpp |
| 12 (STL 컨테이너) | vector, unordered_map, map | homework.cpp |
| 13 (STL 알고리즘) | sort, find_if, count_if | homework.cpp |
| 14 (예외 처리) | LibraryException 계층 | homework2.cpp |
| 15 (파일 I/O) | CSV 저장/불러오기 | homework2.cpp |
| 16 (람다/함수 객체) | 정렬·필터 술어 | homework2.cpp |
| 17 (스마트 포인터) | unique_ptr, shared_ptr | homework.cpp |
| 19 (디자인 패턴) | Singleton(LibraryManager), Observer(알림) | game1.cpp |

## 파일 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 핵심 클래스 설계 — Book/Magazine/Member/Library |
| `homework2.cpp` | 고급 기능 — 파일 I/O, 예외, 람다, 템플릿 검색 |
| `game1.cpp` | 대화형 CLI — 메뉴 기반 통합 시스템 |

## 프로젝트 기능
- 도서/잡지 등록·삭제·검색
- 회원 등록 및 대출·반납 관리
- 대출 기한 초과 감지
- CSV 파일로 데이터 영속화
- 제목·저자·장르 다중 검색
