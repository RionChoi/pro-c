# pro-c 학습 로드맵 V2 — 모던 개발자 과정

> C/C++ 40세션 완료 후 이어지는 실전 기술 스택 학습 과정.
> 이전 과정(C/C++)과 개념 중복 없이, 각 언어/기술의 고유한 특성에 집중한다.

---

## 학습 원칙

- **중복 배제**: 변수, 조건문, 반복문 등 C/C++에서 다룬 공통 개념은 재설명하지 않는다.
- **언어 고유 특성 집중**: 각 기술의 독자적인 패러다임과 생태계에 초점을 맞춘다.
- **실전 프로젝트 중심**: 각 세션마다 현업에서 쓰이는 형태의 결과물을 만든다.
- **세션당 파일**: `README.md` + `homework.py/go/rs` + `homework2.py/go/rs` + `game1.py/go/rs`

---

## Phase 3: Python + FastAPI + LLM API — 10세션

> 목표: AI 서비스 백엔드를 Python으로 구축한다.

| 세션 | 주제 | 핵심 개념 | 상태 |
|------|------|-----------|------|
| 01 | Python 핵심 특성 | 동적 타입, f-string, comprehension, type hint | ⬜ |
| 02 | 함수 고급 패턴 | decorator, generator, *args/**kwargs, functools | ⬜ |
| 03 | 자료구조 심화 | dict/set 고급 활용, collections, itertools | ⬜ |
| 04 | Python OOP | dataclass, dunder 메서드, property, ABC | ⬜ |
| 05 | 모듈 & 패키지 | pip, venv, pyproject.toml, 패키지 구조 | ⬜ |
| 06 | 비동기 프로그래밍 | asyncio, async/await, aiohttp, 동시성 | ⬜ |
| 07 | FastAPI 기초 | 라우팅, Pydantic, 의존성 주입, OpenAPI | ⬜ |
| 08 | FastAPI + DB | SQLAlchemy 2.0, Alembic, 비동기 ORM | ⬜ |
| 09 | FastAPI 인증 | JWT, OAuth2, 미들웨어, 보안 | ⬜ |
| 10 | Claude API 통합 프로젝트 | Anthropic SDK, RAG, 스트리밍, 최종 앱 | ⬜ |

---

## Phase 4: Go — 8세션

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

## Phase 5: Docker + PostgreSQL — 6세션

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

## Phase 6: Rust — 10세션

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

## Phase 7: Kubernetes + Kafka (선택) — 8세션

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

## 진행 기록

| 날짜 | 완료 세션 | 비고 |
|------|-----------|------|
| 2026-05-09 | 로드맵 V2 작성 | Phase 3~7 계획 수립 |
