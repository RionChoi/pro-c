---
name: saas-platform-setup
description: Next.js + Turborepo 기반 멀티 테넌트 SaaS 플랫폼을 처음부터 구축하는 전체 워크플로우 스킬. macOS 개발 환경 구성, 모노레포 초기화, OAuth 2.0 + PKCE 인증, PostgreSQL + Prisma DB 연동까지 단계별로 안내합니다. 사용자가 "SaaS 플랫폼 만들고 싶어", "Next.js 모노레포 세팅", "OAuth 로그인 구현", "Turborepo 초기화", "Prisma PostgreSQL 연동", "K8s SaaS 인프라", "멀티 테넌트 구조" 등을 언급하면 반드시 이 스킬을 사용하세요. 환경 구성부터 인프라까지 전체 플랜을 한 번에 제공합니다.
---

# SaaS Platform Setup Skill

Next.js + Turborepo 기반 멀티 테넌트 SaaS 플랫폼 전체 구축 가이드.

## 전체 구성 개요

```
platform/
├── apps/
│   ├── web-main        # 고객용 SaaS (port 3000)
│   ├── web-admin       # 관리자용 SaaS (port 3001)
│   └── web-partner     # 파트너/벤더용 SaaS (port 3002)
├── services/
│   ├── auth-service    # OAuth 2.0 · PKCE · 토큰 발급
│   ├── api-gateway     # 라우팅 · Rate Limit · JWT 검증
│   └── user-service    # 사용자 · 조직 · 권한 관리
├── packages/
│   ├── ui              # 공유 UI (shadcn/ui)
│   ├── shared-types    # TypeScript 타입 · Zod 스키마
│   └── shared-config/  # ESLint · TSConfig · Prettier
└── infra/
    ├── k8s/
    ├── helm/
    └── github-actions/
```

---

## Phase 1 — macOS 개발 환경 구성

### 설치 상태 확인

```bash
node -v && npm -v && git -v
brew -v && docker -v
kubectl version --client
helm version --short
pnpm -v
```

### 필수 도구 설치

#### Node 버전 매니저 (fnm)

```bash
brew install fnm
echo 'eval "$(fnm env --use-on-cd)"' >> ~/.zshrc
source ~/.zshrc
fnm install 22 && fnm alias default 22
```

> ⚠️ Node v20+ LTS 권장. v25 등 실험적 버전은 패키지 호환 문제 발생 가능.

#### pnpm

```bash
brew install pnpm
pnpm setup
source ~/.zshrc
```

#### Helm

```bash
brew install helm
helm version --short   # v3.x.x 확인
```

#### 로컬 K8s (kind)

```bash
brew install kind
kind create cluster --name platform-local
kubectl cluster-info --context kind-platform-local
```

#### GitOps & 인프라 툴

```bash
brew install argocd k9s kubectx
brew install gh && gh auth login
brew install act
```

---

## Phase 2 — 모노레포 초기화

### Turborepo 프로젝트 생성

```bash
mkdir platform && cd platform
pnpm dlx create-turbo@latest .
# 프롬프트: pnpm 선택
```

### 구조 재편

```bash
mv apps/web apps/web-main
rsync -a --exclude='node_modules' --exclude='.next' apps/web-main/ apps/web-admin/
rsync -a --exclude='node_modules' --exclude='.next' apps/web-main/ apps/web-partner/
rm -rf apps/docs

sed -i '' 's/"name": "web"/"name": "web-main"/' apps/web-main/package.json
sed -i '' 's/"name": "web"/"name": "web-admin"/' apps/web-admin/package.json
sed -i '' 's/"name": "web"/"name": "web-partner"/' apps/web-partner/package.json

sed -i '' 's/next dev --port 3000/next dev --port 3001/' apps/web-admin/package.json
sed -i '' 's/next dev --port 3000/next dev --port 3002/' apps/web-partner/package.json

mkdir -p packages/shared-config
mv packages/eslint-config packages/shared-config/eslint-config
mv packages/typescript-config packages/shared-config/typescript-config
mkdir -p packages/shared-types/src

mkdir -p services/auth-service services/api-gateway services/user-service
mkdir -p infra/k8s infra/helm infra/github-actions
```

### pnpm-workspace.yaml 업데이트

```yaml
packages:
  - "apps/*"
  - "packages/*"
  - "packages/shared-config/*"
  - "services/*"
```

### turbo 로컬 설치

```bash
pnpm add turbo --save-dev -w
pnpm install && pnpm dev
```

---

## Phase 3 — shared-types 패키지

`packages/shared-types/package.json`:

```json
{
  "name": "@repo/shared-types",
  "version": "0.0.0",
  "private": true,
  "exports": { ".": "./src/index.ts" },
  "scripts": { "check-types": "tsc --noEmit" },
  "devDependencies": {
    "@repo/typescript-config": "workspace:*",
    "typescript": "^5"
  }
}
```

앱에 의존성 추가:

```bash
pnpm add @repo/shared-types --filter web-main --workspace
pnpm add @repo/shared-types --filter web-admin --workspace
pnpm add @repo/shared-types --filter web-partner --workspace
```

---

## Phase 4 — OAuth 2.0 + PKCE 인증

```bash
pnpm add next-auth@beta @auth/core --filter web-main
```

`auth.ts`, `middleware.ts`, `app/api/auth/[...nextauth]/route.ts` 생성 후
Google Cloud Console에서 OAuth 앱 등록:

- 승인된 리디렉션 URI: `http://localhost:3000/api/auth/callback/google`

`.env.local`:

```env
AUTH_SECRET=                    # openssl rand -base64 32
AUTH_GOOGLE_ID=
AUTH_GOOGLE_SECRET=
NEXTAUTH_URL=http://localhost:3000
DATABASE_URL="postgresql://user:password@localhost:5432/platform_db"
```

---

## Phase 5 — PostgreSQL + Prisma

```bash
brew install postgresql@16
brew services start postgresql@16
echo 'export PATH="/opt/homebrew/opt/postgresql@16/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
createdb platform_db

psql -d platform_db -c "ALTER USER $(whoami) WITH PASSWORD 'yourpassword';"
psql -d platform_db -c "GRANT ALL PRIVILEGES ON DATABASE platform_db TO $(whoami);"

pnpm add prisma@5 @prisma/client@5 --filter web-main
cd apps/web-main && npx prisma init
npx prisma migrate dev --name init
```

DB 확인:

```bash
psql -d platform_db -c "\dt"
psql -d platform_db -c 'SELECT id, email, name, role, "createdAt" FROM "User";'
```

---

## Phase 6 — GitHub 연동

```bash
git remote add origin https://github.com/YOUR_USERNAME/platform.git
git add .
git commit -m "feat: initialize monorepo structure"
git branch -M main
git push -u origin main
```

---

## 트러블슈팅

실제 구축 과정에서 발생한 에러 전체 정리.

---

### ❌ pnpm PNPM_HOME 경고

**에러**

```
Run "pnpm setup" to create it automatically,
or set the global-bin-dir setting, or the PNPM_HOME env variable.
The global bin directory should be in the PATH.
```

**원인**
`brew install pnpm` 후 전역 바이너리 경로가 쉘 PATH에 미등록.
터미널 재시작 없이 사용하거나 `.zshrc` 반영이 안 된 경우 발생.

**해결**

```bash
pnpm setup
source ~/.zshrc

# 그래도 안 되면 수동 PATH 추가
echo 'export PNPM_HOME="$HOME/Library/pnpm"' >> ~/.zshrc
echo 'export PATH="$PNPM_HOME:$PATH"' >> ~/.zshrc
source ~/.zshrc

which pnpm   # /Users/username/Library/pnpm/pnpm 출력되면 정상
```

---

### ❌ mv: rename apps/web to apps/web-main: No such file or directory

**에러**

```
mv: rename apps/web to apps/web-main/web: No such file or directory
```

**원인**
현재 작업 디렉터리가 `platform/` 루트가 아닌 경우.
또는 이미 이름이 변경된 상태에서 중복 실행한 경우.

**해결**

```bash
pwd          # /Users/username/platform 이어야 함
ls apps/     # web-main이 이미 있으면 이미 완료된 것
cd ~/platform
```

---

### ❌ cp: node_modules 복사 오류

**에러**

```
cp: apps/web-main/node_modules/@repo/ui/node_modules/@repo/eslint-config: No such file or directory
```

**원인**
`cp -r`은 pnpm workspace의 심볼릭 링크 구조를 그대로 복사하려다 경로 오류 발생.
pnpm의 node_modules는 심볼릭 링크 기반이라 일반 cp로 복사 불가.

**해결**
`rsync`로 node_modules와 .next 제외 후 복사:

```bash
rsync -a --exclude='node_modules' --exclude='.next' apps/web-main/ apps/web-admin/
rsync -a --exclude='node_modules' --exclude='.next' apps/web-main/ apps/web-partner/
```

---

### ❌ zsh: command not found:

**에러**

```
zsh: command not found: #
```

**원인**
`#` 주석이 포함된 멀티라인 명령어를 터미널에 한꺼번에 붙여넣으면
zsh가 `#` 이하를 별도 명령으로 인식해 오류 발생.

**해결**
주석 없이 명령어만 실행하거나 한 줄씩 붙여넣기:

```bash
# ❌ 오류 발생
mv apps/web apps/web-main   # web → web-main으로 변경

# ✅ 정상 실행
mv apps/web apps/web-main
```

---

### ❌ ERR_PNPM_WORKSPACE_PKG_NOT_FOUND

**에러**

```
ERR_PNPM_WORKSPACE_PKG_NOT_FOUND
In apps/web-admin: "@repo/typescript-config@workspace:*"
is in the dependencies but no package named "@repo/typescript-config"
is present in the workspace
```

**원인**
`packages/typescript-config`를 `packages/shared-config/typescript-config`로 이동했는데
`pnpm-workspace.yaml`에 `packages/shared-config/*` 경로가 없어
workspace가 해당 패키지를 탐색하지 못함.

**해결**
`pnpm-workspace.yaml`에 중첩 경로 추가:

```yaml
packages:
  - "apps/*"
  - "packages/*"
  - "packages/shared-config/*" # ← 추가
  - "services/*"
```

```bash
pnpm install
```

---

### ❌ turbo 글로벌 버전 경고

**에러**

```
WARNING  No locally installed `turbo` found in your repository.
Using globally installed version (2.9.3), which can cause unexpected behavior.
```

**원인**
`create-turbo`로 생성된 프로젝트는 `package.json`에 turbo 버전만 명시하고
실제 로컬 설치는 되어 있지 않음.
글로벌 turbo와 버전 불일치 시 빌드 동작이 달라질 수 있음.

**해결**

```bash
pnpm add turbo --save-dev -w   # 루트 devDependencies에 로컬 설치
```

---

### ❌ EADDRINUSE: address already in use :::3000

**에러**

```
Error: listen EADDRINUSE: address already in use :::3000
```

**원인 A** — 세 앱이 모두 포트 3000으로 설정되어 동시 실행 시 충돌.

**해결 A**:

```bash
sed -i '' 's/next dev --port 3000/next dev --port 3001/' apps/web-admin/package.json
sed -i '' 's/next dev --port 3000/next dev --port 3002/' apps/web-partner/package.json
```

**원인 B** — 이전에 실행한 서버 프로세스가 여전히 포트를 점유 중.

**해결 B**:

```bash
lsof -i :3000       # PID 확인
kill -9 <PID>       # 프로세스 종료 후 재실행
```

---

### ❌ Prisma v7 datasource url 오류

**에러**

```
Error code: P1012
error: The datasource property `url` is no longer supported in schema files.
Move connection URLs for Migrate to `prisma.config.ts`
```

**원인**
Prisma 7에서 설정 방식이 크게 변경됨.
`schema.prisma`의 `datasource.url`이 deprecated되고
`prisma.config.ts` + `@prisma/adapter-pg` 패턴으로 이동.
설정이 복잡하고 추가 패키지 설치가 필요하며 문법도 버전마다 달라짐.

**해결**
Prisma 5로 다운그레이드 (안정적이고 설정 단순):

```bash
pnpm remove prisma @prisma/client @prisma/adapter-pg
pnpm add prisma@5 @prisma/client@5 --filter web-main
rm prisma.config.ts   # v7 설정 파일 제거
```

`schema.prisma`는 기존 방식 그대로 사용:

```prisma
datasource db {
  provider = "postgresql"
  url      = env("DATABASE_URL")   # v5에서는 정상 작동
}
```

---

### ❌ prisma.config.ts 파싱 오류

**에러**

```
Failed to parse syntax of config file at ".../prisma.config.ts"
```

**원인**
Prisma 7의 `prisma.config.ts` 문법이 마이너 버전마다 달라
`defineConfig` 임포트 경로 또는 adapter 설정이 맞지 않는 경우 발생.
`@prisma/adapter-pg` 미설치 상태에서 adapter 참조 시에도 동일 오류.

**해결**
Prisma 5로 다운그레이드하면 `prisma.config.ts` 자체가 불필요:

```bash
pnpm remove prisma @prisma/client
pnpm add prisma@5 @prisma/client@5 --filter web-main
rm prisma.config.ts
```

---

### ❌ P1010: User was denied access on the database

**에러**

```
Error: P1010: User `` was denied access on the database `postgres.public`
```

**원인**
macOS에서 `createdb`로 생성한 DB는 시스템 유저 소유이지만
비밀번호가 설정되지 않아 Prisma DATABASE_URL 인증 실패.
`postgresql://choi@localhost:5432/platform_db` 형식에서
비밀번호 없이 접속 시 거부됨.

**해결**
PostgreSQL 유저에 비밀번호 설정 후 DATABASE_URL 업데이트:

```bash
psql -d platform_db -c "ALTER USER choi WITH PASSWORD 'choi1234';"
psql -d platform_db -c "GRANT ALL PRIVILEGES ON DATABASE platform_db TO choi;"
```

`.env.local` 수정:

```env
# ❌ 비밀번호 없는 형식 → 접속 거부
DATABASE_URL="postgresql://choi@localhost:5432/platform_db"

# ✅ 비밀번호 포함 형식
DATABASE_URL="postgresql://choi:choi1234@localhost:5432/platform_db"
```

---

### ❌ .env.local 줄바꿈 없이 내용이 붙는 문제

**증상**

```
NEXTAUTH_URL=http://localhost:3000DATABASE_URL="postgresql://..."
```

**원인**
`echo "..." >> .env.local` 방식으로 내용을 추가할 때
파일 마지막 줄에 개행 문자(`\n`)가 없으면 다음 내용이 바로 붙어서 작성됨.
환경 변수 파싱 오류로 이어짐.

**해결**
`cat > file << 'EOF'` 방식으로 전체 파일을 새로 작성:

```bash
cat > .env.local << 'EOF'
AUTH_SECRET=your-secret
AUTH_GOOGLE_ID=your-id
AUTH_GOOGLE_SECRET=your-secret
NEXTAUTH_URL=http://localhost:3000
DATABASE_URL="postgresql://user:password@localhost:5432/platform_db"
EOF
```

---

## Phase 7 — 인증 흐름 확장 (web-admin / web-partner)

web-admin은 `role === "ADMIN"`, web-partner는 `role === "PARTNER"` 유저만 로그인 허용.

각 앱에 동일하게 생성:

- `auth.ts` — Google Provider + role 체크 signIn 콜백
- `middleware.ts` — JWT 검증 · 경로 보호
- `app/api/auth/[...nextauth]/route.ts` — Route Handler
- `lib/prisma.ts` — Prisma 싱글톤
- `app/login/page.tsx` — 로그인 페이지
- `.env.local` — NEXTAUTH_URL 포트 각각 3001, 3002

Google Cloud Console 리디렉션 URI 추가:

```
http://localhost:3001/api/auth/callback/google
http://localhost:3002/api/auth/callback/google
```

---

## Phase 8 — 유저 Role 변경 API

`apps/web-main/app/api/users/route.ts` — 유저 목록 조회 (GET)
`apps/web-main/app/api/users/[id]/role/route.ts` — Role 변경 (PATCH)

Role 변경 테스트:

```bash
curl -X PATCH http://localhost:3000/api/users/<USER_ID>/role \
  -H "Content-Type: application/json" \
  -d '{"role": "ADMIN"}'
```

> ⚠️ 개발 환경에서는 `NODE_ENV === "development"` 체크로 인증 우회 가능.
> 프로덕션에서는 반드시 세션 검증 필요.

---

## Phase 9 — GitHub Actions CI 파이프라인

`.github/workflows/ci.yml`:

```yaml
name: CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]

env:
  FORCE_JAVASCRIPT_ACTIONS_TO_NODE24: true

jobs:
  ci:
    name: Build & Lint & Type Check
    runs-on: ubuntu-latest

    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Setup pnpm
        uses: pnpm/action-setup@v4
        with:
          version: 9.0.0 # package.json의 packageManager 버전과 일치시킬 것

      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: 22
          cache: pnpm

      - name: Install dependencies
        run: pnpm install --frozen-lockfile

      - name: Generate Prisma Client
        run: pnpm --filter web-main exec prisma generate
        env:
          DATABASE_URL: ${{ secrets.DATABASE_URL }}

      - name: Type check
        run: pnpm check-types

      - name: Lint
        run: pnpm lint

      - name: Build
        run: pnpm build
        env:
          AUTH_SECRET: ${{ secrets.AUTH_SECRET }}
          AUTH_GOOGLE_ID: ${{ secrets.AUTH_GOOGLE_ID }}
          AUTH_GOOGLE_SECRET: ${{ secrets.AUTH_GOOGLE_SECRET }}
          DATABASE_URL: ${{ secrets.DATABASE_URL }}
          NEXTAUTH_URL: http://localhost:3000
```

`turbo.json` globalEnv에 환경변수 등록 필수:

```json
"globalEnv": ["NODE_ENV", "AUTH_SECRET", "AUTH_GOOGLE_ID", "AUTH_GOOGLE_SECRET", "DATABASE_URL", "NEXTAUTH_URL"]
```

GitHub Secrets 등록:
👉 https://github.com/USERNAME/platform/settings/secrets/actions

- `AUTH_SECRET`, `AUTH_GOOGLE_ID`, `AUTH_GOOGLE_SECRET`, `DATABASE_URL`

---

## Phase 10 — Docker + kind 로컬 K8s 배포

### Dockerfile (apps/web-main/Dockerfile)

```dockerfile
FROM node:22-alpine AS base

FROM base AS deps
RUN apk add --no-cache libc6-compat
WORKDIR /app

COPY package.json pnpm-lock.yaml pnpm-workspace.yaml ./
COPY apps/web-main/package.json ./apps/web-main/
COPY packages/ui/package.json ./packages/ui/
COPY packages/shared-types/package.json ./packages/shared-types/
COPY packages/shared-config/package.json ./packages/shared-config/
COPY packages/shared-config/typescript-config/package.json ./packages/shared-config/typescript-config/

RUN npm install -g pnpm@9.0.0
RUN pnpm install --frozen-lockfile

FROM base AS builder
WORKDIR /app
COPY --from=deps /app/node_modules ./node_modules
COPY --from=deps /app/apps/web-main/node_modules ./apps/web-main/node_modules
COPY . .

RUN npm install -g pnpm@9.0.0
RUN pnpm --filter web-main exec prisma generate
RUN pnpm --filter web-main build

FROM base AS runner
WORKDIR /app
ENV NODE_ENV=production

RUN addgroup --system --gid 1001 nodejs
RUN adduser --system --uid 1001 nextjs

COPY --from=builder --chown=nextjs:nodejs /app/apps/web-main/.next/standalone ./
COPY --from=builder --chown=nextjs:nodejs /app/apps/web-main/public ./apps/web-main/public
COPY --from=builder --chown=nextjs:nodejs /app/apps/web-main/.next/static ./apps/web-main/.next/static

USER nextjs
EXPOSE 3000
ENV PORT=3000
ENV HOSTNAME="0.0.0.0"

CMD ["node", "apps/web-main/server.js"]
```

`next.config.js`에 standalone 출력 설정 필수:

```js
const nextConfig = { output: "standalone" };
export default nextConfig;
```

### 빌드 & 배포

```bash
# 이미지 빌드
docker build -f apps/web-main/Dockerfile -t platform/web-main:latest .

# kind 클러스터 생성 (Docker Desktop 실행 필수)
kind create cluster --name platform-local

# 이미지 로드
kind load docker-image platform/web-main:latest --name platform-local

# Secret 생성
kubectl create secret generic web-main-secret \
  --from-literal=AUTH_SECRET=<value> \
  --from-literal=AUTH_GOOGLE_ID=<value> \
  --from-literal=AUTH_GOOGLE_SECRET=<value> \
  --from-literal=DATABASE_URL=postgresql://choi:choi1234@host.docker.internal:5432/platform_db

# 배포
kubectl apply -f infra/k8s/web-main/deployment.yaml
kubectl apply -f infra/k8s/web-main/service.yaml

# 상태 확인
kubectl get pods
kubectl get services

# 포트포워딩으로 로컬 접속
kubectl port-forward deployment/web-main 3333:3000
# → http://localhost:3333
```

---

## 트러블슈팅 (추가)

---

### ❌ Next.js 16 searchParams Promise 오류

**에러**

```
Error: Route "/login" used `searchParams.error`.
`searchParams` is a Promise and must be unwrapped with `await` or `React.use()`
```

**원인**
Next.js 16에서 `searchParams`가 동기 객체에서 Promise로 변경됨.
기존 방식으로 직접 접근 시 오류 발생.

**해결**

```typescript
interface Props {
  searchParams: Promise<{ error?: string }>;
}

export default async function LoginPage({ searchParams }: Props) {
  const { error } = await searchParams; // await 필수
  // ...
}
```

---

### ❌ GitHub Actions pnpm 버전 충돌

**에러**

```
Error: Multiple versions of pnpm specified:
  - version 10 in the GitHub Action config with the key "version"
  - version pnpm@9.0.0 in the package.json with the key "packageManager"
```

**원인**
`pnpm/action-setup@v4`의 `version` 값과
`package.json`의 `packageManager` 필드 버전이 불일치.

**해결**

```bash
cat package.json | grep packageManager   # 실제 버전 확인
pnpm -v                                  # 로컬 버전 확인
```

ci.yml의 version을 package.json과 일치시키기:

```yaml
- uses: pnpm/action-setup@v4
  with:
    version: 9.0.0 # package.json의 pnpm@9.0.0과 일치
```

---

### ❌ GitHub Actions shared-types UserRole 중복 export

**에러**

```
Module "./auth" has already exported a member named 'UserRole'.
Consider explicitly re-exporting to resolve the ambiguity.
```

**원인**
`shared-types/src/auth.ts`와 `user.ts` 양쪽에서 `UserRole`을 export해 충돌.

**해결**
`user.ts`에서 중복 정의 제거 후 `auth.ts`에서 re-export:

```typescript
// user.ts
import type { UserRole } from "./auth";
export type { UserRole };
// UserRole 타입 정의 제거
```

---

### ❌ GitHub Actions middleware unused import lint 오류

**에러**

```
Warning: 'NextRequest' is defined but never used @typescript-eslint/no-unused-vars
ESLint found too many warnings (maximum: 0).
```

**원인**
`middleware.ts`에 `NextRequest`를 import했지만 실제로 사용하지 않음.
`--max-warnings 0` 옵션으로 경고도 에러 처리됨.

**해결**

```typescript
// ❌ 사용하지 않는 import 제거
import { auth } from "@/auth";
import { NextResponse } from "next/server";
// NextRequest import 삭제
```

---

### ❌ turbo.json NODE_ENV / 환경변수 미등록 경고

**에러**

```
AUTH_GOOGLE_SECRET is not listed as a dependency in turbo.json
NODE_ENV is not listed as a dependency in turbo.json
```

**원인**
Turborepo는 환경변수를 캐시 키로 사용하므로
`turbo.json`의 `globalEnv`에 등록되지 않은 환경변수 참조 시 경고.
CI에서 lint 오류로 이어짐.

**해결**
`turbo.json`에 `globalEnv` 추가:

```json
{
  "globalEnv": [
    "NODE_ENV",
    "AUTH_SECRET",
    "AUTH_GOOGLE_ID",
    "AUTH_GOOGLE_SECRET",
    "DATABASE_URL",
    "NEXTAUTH_URL"
  ]
}
```

---

### ❌ Dockerfile COPY 경로 오류 (shared-config 구조)

**에러**

```
failed to compute cache key: "/packages/shared-config/eslint-config/package.json": not found
```

**원인**
`packages/shared-config/` 안에 `eslint-config` 서브디렉터리가 없고
`shared-config` 자체가 eslint-config 역할을 하는 구조.
Dockerfile에서 존재하지 않는 경로를 COPY하려다 실패.

**해결**
실제 구조 확인 후 Dockerfile 경로 수정:

```bash
ls packages/shared-config/   # 실제 구조 확인
```

```dockerfile
# ❌ 존재하지 않는 경로
COPY packages/shared-config/eslint-config/package.json ./packages/shared-config/eslint-config/

# ✅ 실제 구조에 맞게
COPY packages/shared-config/package.json ./packages/shared-config/
COPY packages/shared-config/typescript-config/package.json ./packages/shared-config/typescript-config/
```

---

### ❌ Docker kind 클러스터 생성 실패 (Docker daemon 미실행)

**에러**

```
ERROR: failed to create cluster: failed to get docker info
Cannot connect to the Docker daemon at unix:///Users/choi/.docker/run/docker.sock.
Is the docker daemon running?
```

**원인**
kind는 Docker를 런타임으로 사용하므로 Docker Desktop이 실행 중이어야 함.

**해결**

```bash
open -a Docker   # Docker Desktop 실행
# 메뉴바에 고래 아이콘이 뜨고 완전히 실행될 때까지 대기
kind create cluster --name platform-local
```

---

### ❌ Pod CrashLoopBackOff — server.js not found

**에러**

```
Error: Cannot find module '/app/server.js'
```

**원인**
모노레포 standalone 빌드 시 `server.js` 위치가
`/app/server.js`가 아닌 `/app/apps/web-main/server.js`에 생성됨.
Dockerfile CMD 경로 불일치.

**해결**
실제 경로 확인:

```bash
docker run --rm platform/web-main:latest find / -name "server.js" 2>/dev/null | grep -v node_modules
# → /app/apps/web-main/server.js
```

Dockerfile CMD 수정:

```dockerfile
# ❌
CMD ["node", "server.js"]

# ✅
CMD ["node", "apps/web-main/server.js"]
```

---

## Phase 11 — Helm Chart 작성

```
infra/helm/web-main/
├── Chart.yaml
├── values.yaml
└── templates/
    ├── deployment.yaml
    └── service.yaml
```

검증 및 배포:

```bash
helm lint infra/helm/web-main
kubectl delete deployment web-main
kubectl delete service web-main
helm install web-main infra/helm/web-main
helm list
kubectl port-forward deployment/web-main 3333:3000
```

---

## Phase 12 — ArgoCD GitOps 연동

```bash
# ArgoCD 설치
kubectl create namespace argocd
kubectl apply -n argocd -f https://raw.githubusercontent.com/argoproj/argo-cd/stable/manifests/install.yaml
kubectl wait --for=condition=available --timeout=120s deployment/argocd-server -n argocd

# 포트포워딩
kubectl port-forward svc/argocd-server -n argocd 8080:443

# 초기 비밀번호 확인
kubectl -n argocd get secret argocd-initial-admin-secret -o jsonpath="{.data.password}" | base64 -d

# CLI 로그인
argocd login localhost:8080 --username admin --password <password> --insecure

# GitHub 저장소 연결 (Private repo는 PAT 필요)
argocd repo add https://github.com/USERNAME/platform.git \
  --username USERNAME \
  --password <github-pat>

# App 생성
argocd app create web-main \
  --repo https://github.com/USERNAME/platform.git \
  --path infra/helm/web-main \
  --revision main \
  --dest-server https://kubernetes.default.svc \
  --dest-namespace default \
  --helm-set image.pullPolicy=Never \
  --sync-policy automated \
  --auto-prune \
  --self-heal

argocd app get web-main
```

> ⚠️ `argocd app create` 시 `--revision main` 반드시 명시할 것.
> `targetRevision: HEAD`로 하면 Private repo에서 경로를 못 찾는 경우 있음.

---

## Phase 13 — Schema per Tenant 멀티 테넌시

```
platform_db
├── public        (공통 — User, Organization, Tenant)
├── tenant_acme   (테넌트 A 전용)
└── tenant_beta   (테넌트 B 전용)
```

Prisma 스키마에 Tenant 모델 추가 후 마이그레이션:

```bash
npx prisma migrate dev --name add-tenant-model
```

`lib/tenant.ts` — 테넌트 생성 시 자동으로 PostgreSQL 스키마 + 테이블 생성:

```typescript
await prisma.$executeRawUnsafe(`CREATE SCHEMA IF NOT EXISTS "${schemaName}"`);
```

테넌트 API:

- `GET /api/tenants` — 전체 테넌트 목록
- `POST /api/tenants` — 테넌트 생성 (스키마 자동 생성)

DB 확인:

```bash
psql -d platform_db -c "\dn"       # 스키마 목록
psql -d platform_db -c "\dt tenant_acme.*"  # 테넌트 테이블
```

---

## Phase 14 — Kafka 이벤트 스트림 연동

### 로컬 Kafka 실행 (docker-compose.yml)

```yaml
services:
  zookeeper:
    image: confluentinc/cp-zookeeper:7.5.0
    environment:
      ZOOKEEPER_CLIENT_PORT: 2181

  kafka:
    image: confluentinc/cp-kafka:7.5.0
    ports:
      - "9092:9092"
    environment:
      KAFKA_BROKER_ID: 1
      KAFKA_ZOOKEEPER_CONNECT: zookeeper:2181
      KAFKA_ADVERTISED_LISTENERS: PLAINTEXT://localhost:9092
      KAFKA_OFFSETS_TOPIC_REPLICATION_FACTOR: 1
      KAFKA_AUTO_CREATE_TOPICS_ENABLE: true

  kafka-ui:
    image: provectuslabs/kafka-ui:latest
    ports:
      - "9090:8080"
    environment:
      KAFKA_CLUSTERS_0_NAME: platform-local
      KAFKA_CLUSTERS_0_BOOTSTRAPSERVERS: kafka:9092
```

```bash
docker compose up -d
# Kafka UI: http://localhost:9090
```

### 패키지 설치

```bash
pnpm add kafkajs --filter web-main
```

### 토픽 정의 (lib/kafka-topics.ts)

```typescript
export const TOPICS = {
  USER_CREATED: "user.created",
  USER_ROLE_CHANGED: "user.role.changed",
  TENANT_CREATED: "tenant.created",
} as const;
```

### 이벤트 발행 포인트

- 신규 유저 가입 시 → `user.created`
- 테넌트 생성 시 → `tenant.created`

### .env.local 추가

```env
KAFKA_BROKER=localhost:9092
```

### turbo.json globalEnv 추가

```json
"globalEnv": [..., "KAFKA_BROKER"]
```

---

## 트러블슈팅 (추가)

---

### ❌ ArgoCD PermissionDenied

**에러**

```
rpc error: code = PermissionDenied desc = permission denied
```

**원인**
포트포워딩이 끊어지면 ArgoCD CLI 세션이 만료됨.

**해결**

```bash
kubectl port-forward svc/argocd-server -n argocd 8080:443
argocd login localhost:8080 --username admin --password <password> --insecure
```

---

### ❌ ArgoCD app path does not exist

**에러**

```
rpc error: code = InvalidArgument desc = infra/helm/web-main: app path does not exist
```

**원인 A** — Helm Chart가 GitHub에 푸시되지 않은 상태.
**원인 B** — `--revision` 없이 생성 시 Private repo에서 경로 탐색 실패.

**해결**

```bash
git add . && git push origin main   # 먼저 푸시
argocd app create web-main \
  --revision main \                  # revision 명시 필수
  ...
```

---

### ❌ Kafka ESLint any 타입 오류

**에러**

```
Unexpected any. Specify a different type @typescript-eslint/no-explicit-any
```

**원인**
kafkajs의 `$allOperations` 콜백에서 `args`, `query` 타입을 `any`로 선언.

**해결**
`getPrismaForTenant` 함수를 제거하거나 명시적 타입으로 교체.
또는 `search_path`를 raw query로 직접 설정하는 방식 사용.

---

### ❌ 중복 경로에 파일 생성 문제

**증상**

```
apps/web-main/apps/web-main/lib/tenant.ts
```

**원인**
`apps/web-main` 디렉터리 안에서 명령어 실행 시
상대 경로가 중첩되어 잘못된 위치에 파일 생성.

**해결**
항상 `~/platform` 루트에서 명령어 실행:

```bash
pwd   # /Users/username/platform 확인 필수
find apps/web-main -name "tenant.ts"   # 중복 파일 확인
rm -rf apps/web-main/apps              # 잘못된 경로 삭제
```

---

## 현재 완료 상태 (Claude Code 이어받기용)

```
✅ Phase 1  — macOS 개발 환경 구성
✅ Phase 2  — Turborepo 모노레포 초기화
✅ Phase 3  — shared-types 패키지
✅ Phase 4  — OAuth 2.0 + PKCE 인증 (web-main)
✅ Phase 5  — PostgreSQL + Prisma DB 연동
✅ Phase 6  — GitHub 원격 저장소 연결
✅ Phase 7  — web-admin / web-partner 인증 흐름 적용
✅ Phase 8  — 유저 Role 변경 API
✅ Phase 9  — GitHub Actions CI 파이프라인
✅ Phase 10 — Docker + kind 로컬 K8s 첫 배포
✅ Phase 11 — Helm Chart 작성
✅ Phase 12 — ArgoCD GitOps 연동
✅ Phase 13 — Schema per Tenant 멀티 테넌시
✅ Phase 14 — Kafka 이벤트 스트림 연동
```

## Roadmap — 모두 완료 🎉

- [x] web-admin / web-partner 인증 흐름 적용
- [x] 유저 Role 변경 API
- [x] GitHub Actions CI 파이프라인
- [x] kind 로컬 K8s 첫 배포
- [x] Helm Chart 작성
- [x] ArgoCD GitOps 연동
- [x] 멀티 테넌시 DB 전략
- [x] Kafka 이벤트 스트림 연동
