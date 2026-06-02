# CLAUDE.md — Claude Code Project Rules

> Claude Code 전용. Hermes는 AGENTS.md를 참조.
> 마지막 업데이트: 2026-05-02

---

## 1. 역할

Claude Code는 이 프로젝트의 **Brain / Strategic Orchestrator**다.

- 복잡한 설계 판단, 리팩토링, ADR 초안 작성
- 스킬(SKILL.md) 생성/업데이트, 세션 로그 기록
- Graphify/Obsidian 쿼리 패턴 설계
- 장애 진단, 성능 분석

실행(파일시스템 조작, cron, git 커밋)은 Hermes의 영역이다.
Ekko 승인 없이 prod 관련 작업은 절대 수행하지 않는다.

---

## 2. 작업 시작 체크리스트

새 작업 시작 전:
1. `graphify-out/graph.json` 참조 (파일 grep 전에 먼저)
2. `AGENTS.md` §3 Key Decisions 확인
3. 관련 ADR이 있으면 Obsidian `decisions/` 참조
4. 위 참조 후에도 모호하면 → Ekko에게 확인 요청 (§3-1 Think Before Coding 원칙)

---

## 3. 코드 작성 규칙

- TypeScript strict, `any` 금지
- 주석은 WHY가 비자명할 때만 (WHAT 설명 금지)
- 에러 핸들링은 시스템 경계(API 응답, 외부 서비스)에서만
- 새 환경변수 추가 시 반드시 `turbo.json` `globalEnv`에 등록
- Prisma Client: `lib/prisma.ts` 싱글톤. 직접 `new PrismaClient()` 금지
- Server Component 기본. `"use client"` 최소화

---

## 3-1. 코딩 행동 원칙

Andrej Karpathy의 LLM 코딩 관찰에서 도출한 4가지 원칙. 비자명한 작업일수록 엄격히 적용.

### Think Before Coding — 가정 금지, 혼란 노출, 트레이드오프 제시

- 불명확할 때: 가정하고 진행하지 말고 명시적으로 물어볼 것
- 모호한 요구사항: 해석 하나를 묵묵히 선택하지 말고 복수 해석을 제시할 것
- 더 단순한 접근이 보이면: 먼저 push back하고 동의 후 진행
- 혼란스러울 때: 멈추고 무엇이 불분명한지 밝힌 후 확인 요청

### Simplicity First — 요청된 것만, 추측성 코드 금지

- 요청받은 것만 구현. 가상의 미래 요구사항을 위한 기능 추가 금지
- 단일 사용 코드에 추상화 레이어 금지 (단, CQRS·Prisma 싱글톤 등 프로젝트 확립 패턴은 준수)
- 요청하지 않은 유연성/설정 가능성 추가 금지
- 불가능한 시나리오에 대한 에러 핸들링 금지
- 테스트: "시니어 엔지니어가 과도하게 복잡하다고 할까?" → Yes면 단순화

### Surgical Changes — 요청된 곳만 터치, 내 mess만 정리

- 요청과 무관한 인접 코드, 주석, 포매팅 수정 금지
- 작동하는 코드 리팩토링 금지 (요청받지 않은 한)
- 기존 스타일 그대로 따를 것 (개인 선호와 달라도)
- 내 변경으로 생긴 orphan(미사용 import/변수/함수)만 정리
- 기존 dead code 발견 시: 삭제 말고 언급만 할 것 (단, §11 기술 부채 목록에 있는 항목은 예외 — 해당 파일 수정 시 함께 처리)
- 테스트: 변경된 모든 라인이 사용자 요청으로 직접 추적 가능해야 함

### Goal-Driven Execution — 성공 기준 정의 후 검증 루프

성공 기준을 명시하고, 충족될 때까지 루프:

| 명령형 지시 대신 | 검증 가능한 목표로 전환 |
|---|---|
| "유효성 검사 추가" | "잘못된 입력 테스트 작성 → 통과시키기" |
| "버그 수정" | "재현 테스트 작성 → 통과시키기" |
| "X 리팩토링" | "전후 테스트 통과 보장" |

다단계 작업 시 계획 먼저 명시:
```
1. [단계] → 검증: [확인 방법]
2. [단계] → 검증: [확인 방법]
```

---

## 4. 패키지 작업 규칙

- 새 shared 패키지 추가 시 `pnpm-workspace.yaml` 확인
- `@repo/*` import는 workspace:^ 로 참조
- packages/db, packages/auth, packages/shared-types 완료 — 각 앱에서 적극 활용 중
- CQRS 패턴 준수: `QueryResult`, `CommandResult` 활용 (packages/shared-types/src/cqrs.ts)

---

## 4-1. Docker / CD 사전 체크리스트

**push 전 반드시 확인:**

1. **새 workspace 패키지 추가 시** → 해당 패키지를 import하는 앱의 Dockerfile `deps` 스테이지에 `COPY <pkg>/package.json ./<pkg>/` 라인 추가했는지 확인
   - 누락 시 Docker 빌드에서 `Module not found: Can't resolve '@repo/<pkg>'` 에러 발생 (로컬에선 정상 동작해 발견 어려움)
2. **`next.config.js` 확인** → 3개 앱 모두 `output: "standalone"` 있는지 확인
3. **새 환경변수 추가 시** → `turbo.json` `globalEnv` 등록 여부 확인

**현재 `apps/web-main/Dockerfile` deps 스테이지 COPY 목록 (2026-04-30 기준):**
```
COPY packages/ui/package.json ./packages/ui/
COPY packages/shared-types/package.json ./packages/shared-types/
COPY packages/shared-config/package.json ./packages/shared-config/
COPY packages/shared-config/typescript-config/package.json ./packages/shared-config/typescript-config/
COPY packages/db/package.json ./packages/db/
COPY packages/auth/package.json ./packages/auth/
COPY packages/payments/package.json ./packages/payments/   ← 2026-04-30 추가
```
새 패키지를 `web-main`에서 import한다면 이 목록에도 추가할 것.

**현재 구조 (shamefully-hoist=true 적용):**
- `.npmrc`의 `shamefully-hoist=true` 덕분에 모든 패키지가 root `node_modules`에 호이스팅됨
- builder 스테이지는 `COPY --from=deps /app/node_modules ./node_modules` 한 줄만 필요
- 새 패키지 추가해도 Dockerfile builder 스테이지 수정 불필요

---

## 5. Graphify & Hermes Automation

### 5.1 자동 재빌드 파이프라인 (Hermes 담당)

**주간 스케줄:**
- 매주 월요일 03:00 KST (12:00 UTC) → `npx graphify . --full` 자동 실행
- PR 머지 후 자동 → `npx graphify . --incremental` 자동 실행
- 수동 필요 시 → GitHub Actions `workflow_dispatch` 클릭

**Claude Code의 역할:**
- 재빌드 명령은 Hermes에게 위임 (자동 실행됨)
- 파일/함수 검색 전 `graphify-out/graph.json` 먼저 확인
- 비정기적 작업(스키마 변경, 패키지 추가) 시 GitHub Issue로 Hermes의 ask-claude 응답

### 5.2 Hermes Cooperation Protocol (GitHub Actions)

`.github/workflows/hermes-cooperation-protocol.yml` 트리거:
1. 비정기적 작업 감지 (수동 또는 규칙 기반)
2. GitHub Issue 자동 생성 (hermes-inquiry 레이블)
3. Claude Code 검토 후 댓글 승인
4. Hermes 자동 진행

---

## 6. Obsidian & ADR

- 새 아키텍처 결정 시 ADR 초안 작성 (§4.2 템플릿, SKILL.md 참조)
- 저장 경로: `~/obsidian-vault/saas-platform-docs/decisions/ADR-NNN-title.md`
- Ekko 확정 전까지 상태는 `Proposed`

---

## 7. HITL Gates

| Gate | 조건 | 행동 |
|---|---|---|
| G1 | 아키텍처 변경 | ADR 초안 → Ekko 승인 후 진행 |
| G2 | PR 생성 | 내용 작성 → Ekko 리뷰 후 머지 |
| G3 | dev 배포 | main 머지 시 자동 |
| G4 | staging/prod | 절대 불가 — Ekko 전담 |

---

## 8. 세션 종료 시

`~/.claude/skills/saas-platform-setup/session-log/YYYY-MM-DD.md` 업데이트:
- 주요 작업 (3-5줄 bullet)
- 생성/수정 파일 목록
- 결정사항 → ADR 링크
- 열린 이슈 / TODO

---

## 9. Vercel 배포 구조

앱마다 **별도 Vercel 프로젝트** 필요. 단일 프로젝트로 빌드 시 루트에서 `.next`를 찾아 에러 발생.

| 앱 | Root Directory | Build Command |
|---|---|---|
| web-main | `apps/web-main` | `cd ../.. && pnpm turbo run build --filter=web-main...` |
| web-admin | `apps/web-admin` | `cd ../.. && pnpm turbo run build --filter=web-admin...` |
| web-partner | `apps/web-partner` | `cd ../.. && pnpm turbo run build --filter=web-partner...` |

- Output Directory: `.next` (공통)
- Install Command: `cd ../.. && pnpm install` (공통)
- Vercel Team: `haegunchois-projects`

---

## 10. 절대 금지

- `.env*` 파일 읽기/쓰기
- `prisma migrate deploy` (prod 대상)
- `git push --force` to main
- staging/prod 환경변수 수정
- `graphify-out/graph.json` 직접 편집

## 11. 알려진 이슈 / 기술 부채

빌드·런타임에서 발견된 미해결 문제. 작업 중 해당 파일 수정 시 함께 처리할 것.

### ✅ middleware → proxy 파일 컨벤션 변경 (Next.js 16.2.0) — 2026-04-30 해결

- 3개 앱 모두 `middleware.ts` → `proxy.ts` 파일명 변경, 함수명 `middleware` → `proxy` 변경 완료
- 신규 proxy 파일 작성 시 파일명 `proxy.ts`, 함수명 `proxy`로 작성할 것

### ✅ `@repo/auth` 바렐 export에 Node.js crypto 포함 (Edge Runtime 경고) — 2026-04-30 해결

- `packages/auth/src/edge.ts` 생성: `hasRole`, `Role`, `requireRole` 등 Edge-safe export만 포함
- `packages/auth/package.json`에 `"./edge": "./src/edge.ts"` 서브패스 추가
- Edge Runtime(proxy 파일 등)에서는 `@repo/auth` 대신 `@repo/auth/edge` 또는 `@repo/auth/rbac` 에서 import할 것

---

## graphify

This project has a graphify knowledge graph at graphify-out/.

Rules:
- Before answering architecture or codebase questions, read graphify-out/GRAPH_REPORT.md for god nodes and community structure
- If graphify-out/wiki/index.md exists, navigate it instead of reading raw files
- After modifying code files in this session, run `graphify update .` to keep the graph current (AST-only, no API cost)

# CLAUDE.md

Behavioral guidelines to reduce common LLM coding mistakes. Merge with project-specific instructions as needed.

**Tradeoff:** These guidelines bias toward caution over speed. For trivial tasks, use judgment.

## 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:
- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

## 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:
- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

## 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:
- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

---

**These guidelines are working if:** fewer unnecessary changes in diffs, fewer rewrites due to overcomplication, and clarifying questions come before implementation rather than after mistakes.
