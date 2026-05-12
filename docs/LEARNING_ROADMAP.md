# pro-c 학습 로드맵 (전체 과정)

## 목표
- C → C++ → Python → Go → Docker/PostgreSQL → Rust → Kubernetes/Kafka 순으로 학습한다.
- 각 언어·기술의 고유한 특성에 집중하고, 이미 배운 공통 개념은 반복하지 않는다.
- 세션마다 현업에서 쓰이는 형태의 결과물을 만든다.

## 세션 파일 구성
| 파일 | 역할 |
|------|------|
| `README.md` | 학습 내용 요약 (한국어) |
| `homework.*` | 과제 1 — 핵심 연습 |
| `homework2.*` | 과제 2 — 확장 연습 |
| `game1.*` | 미니 프로젝트 — 학습 내용 응용 |

## 디렉토리 구조
```
pro-c/
├── c-lang/           # Phase 1 완료 ✅
├── cpp-lang/         # Phase 2 완료 ✅
├── python-lang/      # Phase 3 (진행 중)
├── go-lang/          # Phase 4
├── docker-postgres/  # Phase 5
├── rust-lang/        # Phase 6
└── k8s-kafka/        # Phase 7 (선택)
```

---

## Phase 1: C 언어 (c-lang/) — 20세션 ✅

| 세션 | 주제 | 상태 |
|------|------|------|
| 01 | 변수와 입출력 (Variables & I/O) | ✅ |
| 02 | 조건문과 반복문 (Conditions & Loops) | ✅ |
| 03 | 함수와 배열 (Functions & Arrays) | ✅ |
| 04 | 포인터와 문자열 (Pointers & Strings) | ✅ |
| 05 | 구조체 (Structs) | ✅ |
| 06 | Enum & Typedef | ✅ |
| 07 | 동적 메모리 (Dynamic Memory) | ✅ |
| 08 | 파일 I/O (File I/O) | ✅ |
| 09 | 전처리기 & 헤더 (Preprocessor & Headers) | ✅ |
| 10 | 비트연산 & 고급 (Bitwise & Advanced) | ✅ |
| 11 | 연결 리스트 (Linked List) | ✅ |
| 12 | 정렬 & 탐색 (Sorting & Searching) | ✅ |
| 13 | 에러 처리 (Error Handling) | ✅ |
| 14 | 스택 & 큐 (Stack & Queue) | ✅ |
| 15 | 재귀 (Recursion) | ✅ |
| 16 | 함수 포인터 & 콜백 (Function Pointers) | ✅ |
| 17 | 멀티파일 프로젝트 (Multi-file Project) | ✅ |
| 18 | 커맨드라인 인자 (Command-line Args) | ✅ |
| 19 | 해시 테이블 (Hash Table) | ✅ |
| 20 | C 최종 프로젝트 (Final Project) | ✅ |

---

## Phase 2: C++ (cpp-lang/) — 20세션 ✅

| 세션 | 주제 | 상태 |
|------|------|------|
| 01 | C++ 기초 & iostream | ✅ |
| 02 | 문자열 & 네임스페이스 (String & Namespace) | ✅ |
| 03 | 제어 흐름 (Control Flow) | ✅ |
| 04 | 함수 & 오버로딩 (Functions & Overloading) | ✅ |
| 05 | 배열 & 벡터 (Arrays & Vectors) | ✅ |
| 06 | 포인터 & 참조 (Pointers & References) | ✅ |
| 07 | OOP — 클래스 (Classes) | ✅ |
| 08 | 생성자 & 소멸자 (Constructors & Destructors) | ✅ |
| 09 | 상속 & 다형성 (Inheritance & Polymorphism) | ✅ |
| 10 | 연산자 오버로딩 (Operator Overloading) | ✅ |
| 11 | 템플릿 (Templates) | ✅ |
| 12 | STL 컨테이너 (STL Containers) | ✅ |
| 13 | STL 알고리즘 (STL Algorithms) | ✅ |
| 14 | 예외 처리 (Exception Handling) | ✅ |
| 15 | 파일 I/O & 스트림 (File I/O & Streams) | ✅ |
| 16 | 람다 & 함수 객체 (Lambda & Functors) | ✅ |
| 17 | 스마트 포인터 & 이동 시맨틱 (Smart Pointers & Move) | ✅ |
| 18 | 멀티스레딩 기초 (Multithreading Basics) | ✅ |
| 19 | 디자인 패턴 (Design Patterns) | ✅ |
| 20 | C++ 최종 프로젝트 (Final Project) | ✅ |

---

## Phase 3: Python + FastAPI + LLM API (python-lang/) — 10세션

> 목표: AI 서비스 백엔드를 Python으로 구축한다.

| 세션 | 주제 | 핵심 개념 | 상태 |
|------|------|-----------|------|
| 01 | Python 핵심 특성 | 동적 타입, f-string, comprehension, type hint | ✅ |
| 02 | 함수 고급 패턴 | decorator, generator, *args/**kwargs, functools | ✅ |
| 03 | 자료구조 심화 | dict/set 고급 활용, collections, itertools | ✅ |
| 04 | Python OOP 고유 특성 | dataclass, dunder 메서드, property, ABC (C++ OOP 중복 제외) | ✅ |
| 05 | 모듈 & 패키지 | pip, venv, pyproject.toml, 패키지 구조 | ⬜ |
| 06 | 비동기 프로그래밍 | asyncio 이벤트 루프, async/await, aiohttp (스레드/락 중복 제외) | ⬜ |
| 07 | FastAPI 기초 | 라우팅, Pydantic, 의존성 주입, OpenAPI | ⬜ |
| 08 | FastAPI + DB | SQLAlchemy 2.0, Alembic, 비동기 ORM | ⬜ |
| 09 | FastAPI 인증 | JWT, OAuth2, 미들웨어, 보안 | ⬜ |
| 10 | Claude API 통합 프로젝트 | Anthropic SDK, RAG, 스트리밍, 최종 앱 | ⬜ |

---

## Phase 4: Go (go-lang/) — 8세션

> 목표: 클라우드 네이티브 마이크로서비스를 Go로 구축한다.

| 세션 | 주제 | 핵심 개념 | 상태 |
|------|------|-----------|------|
| 01 | Go 핵심 특성 | 패키지, 다중 반환값, 슬라이스, 맵 | ⬜ |
| 02 | Go 인터페이스 & 타입 | 인터페이스, 타입 임베딩, 타입 단언 | ⬜ |
| 03 | 에러 처리 & defer | Go 에러 패턴, defer/panic/recover | ⬜ |
| 04 | 고루틴 & 채널 | goroutine, channel, select, WaitGroup | ⬜ |
| 05 | 표준 라이브러리 | net/http, encoding/json, os, bufio | ⬜ |
| 06 | REST API 서버 | net/http 순수 구현, 라우터, 미들웨어 | ⬜ |
| 07 | Go + PostgreSQL | database/sql, sqlx, 마이그레이션 | ⬜ |
| 08 | Go 최종 프로젝트 | 인증 포함 마이크로서비스 API | ⬜ |

---

## Phase 5: Docker + PostgreSQL (docker-postgres/) — 6세션

> 목표: 모든 프로젝트를 컨테이너화하고 DB를 프로덕션 수준으로 운용한다.

| 세션 | 주제 | 핵심 개념 | 상태 |
|------|------|-----------|------|
| 01 | Docker 핵심 | 이미지, 컨테이너, Dockerfile, 레이어 | ⬜ |
| 02 | Docker Compose | 멀티 컨테이너, 네트워크, 볼륨 | ⬜ |
| 03 | PostgreSQL 심화 | 인덱스, 트랜잭션, EXPLAIN, 최적화 | ⬜ |
| 04 | 컨테이너화 실전 | Python/Go 앱 Docker 이미지 빌드 | ⬜ |
| 05 | CI/CD 기초 | GitHub Actions, 자동 빌드/테스트/배포 | ⬜ |
| 06 | 최종 배포 프로젝트 | FastAPI + Go + PostgreSQL 풀스택 배포 | ⬜ |

---

## Phase 6: Rust (rust-lang/) — 10세션

> 목표: 소유권 시스템을 이해하고 고성능 시스템 도구를 제작한다.

| 세션 | 주제 | 핵심 개념 | 상태 |
|------|------|-----------|------|
| 01 | Rust 소유권 & 빌림 | ownership, borrowing, lifetime 기초 | ⬜ |
| 02 | 타입 시스템 | struct, enum, Option, Result | ⬜ |
| 03 | 트레이트 | trait, 제네릭, 트레이트 바운드 | ⬜ |
| 04 | 컬렉션 & 이터레이터 | Vec, HashMap, 이터레이터 어댑터 | ⬜ |
| 05 | 에러 처리 | ?, thiserror, anyhow | ⬜ |
| 06 | 모듈 & 카고 | mod, pub, Cargo.toml, crates.io | ⬜ |
| 07 | 동시성 | thread, Arc, Mutex, Rayon | ⬜ |
| 08 | 비동기 Rust | async/await, Tokio, Future | ⬜ |
| 09 | CLI 도구 제작 | clap, std::fs, 파일 처리 도구 | ⬜ |
| 10 | Rust 최종 프로젝트 | 멀티스레드 HTTP 서버 또는 데이터 파이프라인 | ⬜ |

---

## Phase 7: Kubernetes + Kafka (k8s-kafka/) — 8세션 (선택)

> 목표: 대규모 분산 시스템 설계와 운영을 이해한다.

| 세션 | 주제 | 핵심 개념 | 상태 |
|------|------|-----------|------|
| 01 | Kubernetes 핵심 | Pod, Deployment, Service, ConfigMap | ⬜ |
| 02 | K8s 스토리지 & 네트워크 | PV, PVC, Ingress, NetworkPolicy | ⬜ |
| 03 | Helm & 패키지 관리 | Helm 차트, 릴리즈, 저장소 | ⬜ |
| 04 | Apache Kafka 기초 | 토픽, 파티션, Producer, Consumer | ⬜ |
| 05 | Kafka Streams | 스트림 처리, 상태 저장, 윈도잉 | ⬜ |
| 06 | K8s + Kafka 통합 | Strimzi, K8s에서 Kafka 운영 | ⬜ |
| 07 | 모니터링 | Prometheus, Grafana, 알림 설정 | ⬜ |
| 08 | 최종 프로젝트 | 실시간 데이터 파이프라인 K8s 배포 | ⬜ |

---

## 진행 기록

| 날짜 | 완료 세션 | 비고 |
|------|-----------|------|
| 2026-04-10 | C-01 ~ C-04 | 초기 세션 완료 |
| 2026-04-19 | C-05, C-06 | 구조체, Enum & Typedef |
| 2026-04-21 | C-07, C-08 | 동적 메모리, 파일 I/O |
| 2026-04-22 | C-09, C-10 | 전처리기/헤더, 비트연산 |
| 2026-04-23 | C-11, C-12 | 연결 리스트, 정렬 & 탐색 |
| 2026-04-23 | C-13, C-14 | 에러 처리, 스택 & 큐 |
| 2026-04-24 | C-15, C-16 | 재귀, 함수 포인터 & 콜백 |
| 2026-04-25 | C-17, C-18 | 멀티파일 프로젝트, 커맨드라인 인자 |
| 2026-04-26 | C-19, C-20 | 해시 테이블, C 최종 프로젝트 |
| 2026-04-27 | C++-01, C++-02 | C++ 기초 입출력, 문자열 & 네임스페이스 |
| 2026-04-28 | C++-03, C++-04 | 제어 흐름, 함수 & 오버로딩 |
| 2026-04-29 | C++-05, C++-06 | 배열 & 벡터, 포인터 & 참조 |
| 2026-04-30 | C++-07, C++-08 | OOP - 클래스, 생성자 & 소멸자 |
| 2026-05-01 | C++-09, C++-10 | 상속 & 다형성, 연산자 오버로딩 |
| 2026-05-02 | C++-11, C++-12 | 템플릿, STL 컨테이너 |
| 2026-05-03 | C++-13, C++-14 | STL 알고리즘, 예외 처리 |
| 2026-05-04 | C++-15, C++-16 | 파일 I/O & 스트림, 람다 & 함수 객체 |
| 2026-05-05 | C++-17, C++-18 | 스마트 포인터 & 이동 시맨틱, 멀티스레딩 기초 |
| 2026-05-07 | C++-19 | 디자인 패턴 (Singleton, Observer, Strategy) |
| 2026-05-08 | C++-20 | C++ 최종 프로젝트 — 도서관 관리 시스템 |
| 2026-05-09 | Python-01 | 핵심 특성 (동적 타입, f-string, comprehension) |
| 2026-05-10 | Python-02 | 함수 고급 패턴 (decorator, generator, functools) |
| 2026-05-11 | Python-03 | 자료구조 심화 (collections, itertools) |
| 2026-05-12 | Python-04 | Python OOP 고유 특성 (dataclass, property, ABC, Protocol, slots) |
