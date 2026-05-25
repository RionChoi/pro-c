# AGENTS.md — Platform AI Context

> 이 파일은 Claude Code, Hermes 등 모든 AI 행위자가 읽는 공유 컨텍스트다.
> 변경 시 Claude Code가 업데이트하고 Git에 커밋한다.
> 마지막 업데이트: 2026-04-18

---

## 1. Project Overview

멀티테넌트 SaaS 플랫폼. 테넌트별 스키마 격리(schema-per-tenant) 방식 적용.
단독 개발자: Ekko. AI 협업: Claude Code (Brain) + Hermes (Hands).

**Tech Stack**
- Runtime: Node.js 20 / TypeScript
- Monorepo: Turborepo + pnpm
- Frontend: Next.js 16.2 (App Router, Server Components)
- Auth: NextAuth v5 (Google OAuth 2.0)
- DB: PostgreSQL (local: Railway, schema-per-tenant)
- ORM: Prisma 5
- Message Queue: Kafka (Confluent, via Docker locally)
- CI/CD: GitHub Actions → Vercel (frontend) + Railway (DB)

---

## 2. Project Structure

```
/Users/choi/platform/
├── apps/
│   ├── web-main/      Next.js 16.2 — 메인 서비스 포털 (port 3000)
│   ├── web-admin/     Next.js 16.2 — 어드민 포털
│   └── web-partner/   Next.js 16.2 — 파트너 포털
├── packages/
│   ├── db/            Prisma schema 중앙화, PrismaClient singleton (@repo/db)
│   ├── auth/          NextAuth 공유 설정, createAuthConfig() 팩토리 (@repo/auth)
│   ├── ui/            shadcn/ui 기반 공유 컴포넌트
│   ├── shared-types/  공유 TypeScript 타입 (api.ts, auth.ts, user.ts)
│   └── shared-config/ ESLint, TypeScript 설정 preset
├── infra/
│   ├── github-actions/ CI 워크플로우
│   ├── helm/           Helm chart (web-main)
│   └── k8s/            ArgoCD, K8s manifest
├── graphify-out/      Graphify 출력 (graph.json, GRAPH_REPORT.md) — Git 포함
├── docker-compose.yml Kafka + Zookeeper + Kafka UI (local)
├── turbo.json
├── pnpm-workspace.yaml
├── AGENTS.md          ← 이 파일
├── CLAUDE.md          Claude Code 전용 규칙
└── README.md
```

### 2.1 패키지 현황

| 패키지 | 상태 | 설명 |
|---|---|---|
| `packages/db` | ✅ 완료 | Prisma schema + migrations 중앙화 (2026-04-18) |
| `packages/auth` | ✅ 완료 | createAuthConfig() 팩토리, NextAuth 공유 (2026-04-18) |
| `packages/ui` | 운영중 | shadcn/ui 기반 공유 컴포넌트 |
| `packages/shared-types` | 운영중 | 공유 TypeScript 타입 |
| `packages/shared-config` | 운영중 | ESLint, TypeScript preset |

---

## 3. Key Architectural Decisions

| 결정 | 내용 | ADR |
|---|---|---|
| Hermes SDLC | Claude Code(Brain) + Hermes(Hands) 역할 분담 | ADR-001 |
| Harness 제거 | 과잉 복잡도. GitHub Actions + Vercel + Railway로 대체 | ADR-002 |
| Graphify 도입 | 코드 구조 지식 그래프. graphify-out/ Git 포함 | ADR-003 |
| Obsidian 도입 | ~/obsidian-vault/saas-platform-docs/ 4폴더 ADR 기록 | ADR-004 |
| PostgreSQL | schema-per-tenant, pgvector Hybrid RAG는 Month 3+ | ADR-005 |
| 3-앱 구조 | web-main / web-admin / web-partner 분리 (apps/api 불필요) | ADR-006 |
| packages/db·auth 중앙화 | Prisma + NextAuth 공유 패키지 완료 (2026-04-18) | ADR-007 |
| Prod 배포 전략 | deploy/prod 브랜치 분리, ArgoCD 수동 sync, GitHub Environment 이중 게이트 | ADR-008 |

---

## 4. Cooperation Protocol (이중 브레인 검증)

Hermes(Hands)는 모든 실무 작업(파일 수정, 인프라 변경 등)을 수행하기 전, 반드시 다음 절차를 거쳐야 한다.

1.  **전략 요청**: `ask-claude`와 `ask-gemini`를 호출하여 현재 작업에 대한 설계 가이드와 위험 요소를 파악한다.
2.  **교차 검증 (Conform)**: 두 브레인의 의견이 일치하는지 확인한다. 만약 의견이 다를 경우, 두 의견을 종합하여 최적의 절충안을 사용자에게 제안하거나, 더 타당한 논리를 따른다.
3.  **최종 승인**: 두 브레인의 검증이 완료된 후에만 실제 `Action`을 수행한다.

---

## 5. Conventions

### 4.1 파일 네이밍
- 컴포넌트: PascalCase (`UserCard.tsx`)
- 유틸/훅: camelCase (`useTenant.ts`)
- 라우트 핸들러: `route.ts` (Next.js 규칙)
- 환경변수: SCREAMING_SNAKE_CASE

### 4.2 코딩 규칙
- TypeScript strict mode. `any` 사용 금지.
- `@repo/*` workspace import를 외부 패키지보다 우선.
- Prisma Client는 `@repo/db`에서 import. 앱 내 직접 인스턴스화 금지.
- Server Component 기본, 필요 시만 `"use client"`.

### 4.3 환경변수 (turbo.json globalEnv 등록 필수)
```
AUTH_SECRET, AUTH_GOOGLE_ID, AUTH_GOOGLE_SECRET
DATABASE_URL
NEXTAUTH_URL
KAFKA_BROKER
```

---

## 5. Graphify Management

- **실행 주체**: Hermes (실행) / Claude Code (전략 판단)
- **출력 경로**: `graphify-out/` — Git에 포함, 수동 편집 금지
- **재빌드 트리거**: 매주 월요일 03:00 (cron), PR 머지 직후, packages/ 구조 변경
- **파일 검색 전에 `graphify-out/graph.json` 먼저 참조할 것**
- Full rebuild 하루 3회 이내

---

## 6. Obsidian Vault

- **경로**: `~/obsidian-vault/saas-platform-docs/`
- **구조**: `decisions/` · `postmortems/` · `knowledge/` · `prompts/`
- ADR 작성: Claude Code 초안 → Ekko 확정 → Git commit
- 모든 아키텍처 결정은 ADR로 기록 (구두 합의 금지)

---

## 7. CI/CD & Environments

| 환경 | 배포 방법 | 승인자 |
|---|---|---|
| dev (preview) | GitHub Actions → Vercel 자동 | 자동 |
| staging | 수동 | Ekko |
| prod | 수동, 최종 | Ekko 전담 |

**GitHub Actions** (`.github/workflows/ci.yml`): lint → type-check → build

---

## 8. Forbidden Actions (AI 공통)

- `git push --force` to main
- Secrets 조회/수정 (`.env*` 파일 읽기 포함)
- Production 환경변수 변경
- `prisma migrate deploy` to prod
- staging/prod 배포 실행
- `graph.json` 수동 편집
- `node_modules/` 직접 수정

---

## 9. 세션 종료 시 자동 정리 (AI 공통, 필수)

모든 AI 행위자는 **대화 종료 시** 아래 내용을 자동으로 정리·저장해야 한다. 사용자가 요청하지 않아도 반드시 수행할 것.

1. **Knowledge Item 업데이트**: `~/.gemini/antigravity/knowledge/platform-status/artifacts/status.md`에 다음을 반영
   - 새로 완료된 작업
   - 새로운 미해결 이슈
   - 아키텍처 결정 변경사항
   - 다음 단계 우선순위 변경

2. **업데이트 기준**: 프로젝트 관련 코드 변경, 구조 결정, 이슈 발견이 있었던 대화에서만. 단순 질문/답변만 있었던 대화는 생략 가능.

3. **AGENTS.md 동기화**: 아키텍처 구조나 패키지 현황이 변경되면 AGENTS.md §2, §3도 함께 업데이트.

## graphify

This project has a graphify knowledge graph at graphify-out/.

Rules:
- Before answering architecture or codebase questions, read graphify-out/GRAPH_REPORT.md for god nodes and community structure
- If graphify-out/wiki/index.md exists, navigate it instead of reading raw files
- After modifying code files in this session, run `graphify update .` to keep the graph current (AST-only, no API cost)
