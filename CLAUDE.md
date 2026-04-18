# CLAUDE.md — Claude Code Project Rules

> Claude Code 전용. Hermes는 AGENTS.md를 참조.
> 마지막 업데이트: 2026-04-18

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

---

## 3. 코드 작성 규칙

- TypeScript strict, `any` 금지
- 주석은 WHY가 비자명할 때만 (WHAT 설명 금지)
- 에러 핸들링은 시스템 경계(API 응답, 외부 서비스)에서만
- 새 환경변수 추가 시 반드시 `turbo.json` `globalEnv`에 등록
- Prisma Client: `lib/prisma.ts` 싱글톤. 직접 `new PrismaClient()` 금지
- Server Component 기본. `"use client"` 최소화

---

## 4. 패키지 작업 규칙

- 새 shared 패키지 추가 시 `pnpm-workspace.yaml` 확인
- `@repo/*` import는 workspace:^ 로 참조
- packages/db, packages/auth는 아직 미구현 — 현재 각 앱의 `lib/prisma.ts`, `auth.ts` 직접 수정

---

## 5. Graphify 사용

- 파일/함수 검색 전 `graphify-out/graph.json` 먼저 확인
- 재빌드 명령은 Hermes에게 위임 (`npx graphify . --incremental`)
- 재빌드 필요 판단만 Claude Code가 수행

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

## 9. 절대 금지

- `.env*` 파일 읽기/쓰기
- `prisma migrate deploy` (prod 대상)
- `git push --force` to main
- staging/prod 환경변수 수정
- `graphify-out/graph.json` 직접 편집
