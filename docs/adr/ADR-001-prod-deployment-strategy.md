# ADR-001: Production 배포 전략 및 게이트 구조

**Status:** Proposed
**Date:** 2026-05-10
**Deciders:** Ekko (최종), Claude Code (초안)
**Related:** CLAUDE.md §7 (HITL Gates), AGENTS.md §G4

---

## Context

현재 `staging` 환경은 ArgoCD + `deploy/staging` 브랜치 기반 GitOps로 자동화되어 있으나, **production 환경은 미구성 상태**다. 다음 항목이 누락되어 있다.

- `values-prod.yaml`에 `image.tag` 미명시 (base `values.yaml`의 `latest`로 폴백 → 재현성 없음)
- ArgoCD `Application` 정의에 prod 타겟 없음 (staging만 존재)
- Ingress host가 `app.example.com` 플레이스홀더
- CI/CD에 prod 승격 워크플로 없음
- prod DB migration 정책 없음 (CI는 staging만 자동 적용)

`CLAUDE.md` §10 · §7 G4 및 `AGENTS.md`는 prod 배포가 **Ekko 수동 전담** 영역임을 명시한다. 자동화는 가능하되 **트리거는 항상 인간 승인**을 거쳐야 한다.

---

## Decision

### 1. Image 태깅 전략

- **Release tag 기반 승격**: `v*.*.*` 형식 git tag 푸시 → prod 빌드 트리거
- staging의 `sha-*` 태그는 prod 승격 대상이 아님 (재태깅하지 않음)
- 단, 동일 이미지를 staging에서 검증 후 prod로 승격하기 위해 **SHA 기반 promotion** 옵션도 병행:
  - `workflow_dispatch` 입력으로 staging SHA 지정 → prod helm values 업데이트

### 2. ArgoCD prod Application

- 별도 브랜치 **`deploy/prod`** 생성 (staging과 분리)
- `infra/k8s/argocd-app-prod.yaml` 신규 (3개 앱)
  - `targetRevision: deploy/prod`
  - `syncPolicy.automated` **비활성화** (수동 sync only) — G4 게이트 강제

### 3. CI/CD 워크플로

- 신규 `.github/workflows/cd-prod.yml`
  - 트리거: `workflow_dispatch` (입력: staging SHA 또는 release tag)
  - 단계: 이미지 재태깅(`prod`) → `deploy/prod` 브랜치 업데이트 → ArgoCD 수동 sync 안내
- prod migration: **별도 잡** (`migrate-prod`), `workflow_dispatch`로만 실행, `STAGING_DATABASE_URL` 미사용

### 4. 도메인 / TLS

- 실제 도메인 확정 시점에 `values-prod.yaml`의 `ingress.host` 업데이트
- `cert-manager` `letsencrypt-prod` ClusterIssuer 사용 (이미 명시됨)

### 5. 롤백 전략

- `deploy/prod` 브랜치 revert → ArgoCD 수동 sync
- DB migration은 forward-only, 롤백 필요 시 별도 ADR

### 6. G4 게이트 강제 메커니즘

- `cd-prod.yml`에 `environment: production` 지정 → GitHub Environment **Required reviewers**에 Ekko 등록
- ArgoCD prod app의 `automated` 비활성화로 이중 게이트

---

## Consequences

### Positive

- staging/prod 완전 분리 → staging 실험이 prod에 영향 없음
- Ekko 승인 게이트가 GitHub Environment 레벨에서 강제됨 (워크플로 우회 불가)
- 동일 SHA 이미지를 prod로 승격 → 검증된 아티팩트 재사용

### Negative / Risk

- 운영 복잡도 증가: 브랜치 2개(`deploy/staging`, `deploy/prod`), Application 6개
- prod DB migration 수동 운영 부담
- Sealed Secret prod 키 별도 발급 필요

### Neutral

- staging은 기존 자동화 그대로 유지 (변경 없음)

---

## Alternatives Considered

| 대안 | 채택 안 한 이유 |
|---|---|
| Single branch (`deploy/main`) + env folder | 환경 격리 약함, 실수로 prod 영향 가능 |
| Argo Rollouts (canary/blue-green) | 현 단계 트래픽 규모로 과잉, 추후 ADR로 도입 검토 |
| Tag-only promotion (`workflow_dispatch` 미사용) | 핫픽스 SHA promotion 불편 |

---

## Implementation Checklist (Ekko 승인 후)

- [ ] `infra/k8s/argocd-app-prod.yaml` 신규 작성
- [ ] `infra/helm/*/values-prod.yaml`에 `image.tag` placeholder 추가
- [ ] `.github/workflows/cd-prod.yml` 신규
- [ ] GitHub Environment `production` 생성 + Required reviewer 등록
- [ ] prod 도메인 DNS 설정
- [ ] Sealed Secret prod 키 발급 + `*-prod-sealed-secret.yaml` 추가
- [ ] `CLAUDE.md` §9 Vercel 표 → K8s prod 표로 추가
- [ ] runbook 작성 (`docs/runbooks/prod-deploy.md`)
