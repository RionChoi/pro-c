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

### ❌ zsh: command not found: #

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
  - "packages/shared-config/*"   # ← 추가
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

## Phase 7 — web-admin / web-partner 인증 흐름 적용

### 개요

web-main의 OAuth 인증 구조를 web-admin(관리자)과 web-partner(파트너)에 확장.
각 앱은 **역할(Role) 기반 접근 제어**를 통해 해당 역할의 사용자만 로그인 허용.

| 앱 | 포트 | 허용 역할 | 테마 색상 |
|----|------|-----------|-----------|
| web-main | 3000 | 전체 | Blue |
| web-admin | 3001 | ADMIN | Indigo |
| web-partner | 3002 | PARTNER | Emerald |

### 패키지 설치

```bash
pnpm add next-auth@beta @auth/core @prisma/client@5 prisma@5 --filter web-admin
pnpm add next-auth@beta @auth/core @prisma/client@5 prisma@5 --filter web-partner
```

### 생성된 파일 구조

각 앱에 동일한 구조로 다음 파일 생성:

```
apps/web-admin/          (또는 web-partner/)
├── auth.ts              # NextAuth 설정 (역할 체크 포함)
├── middleware.ts         # 인증 미들웨어
├── lib/
│   └── prisma.ts        # Prisma 싱글톤 클라이언트
├── prisma/
│   └── schema.prisma    # DB 스키마 (web-main과 동일)
├── app/
│   ├── api/auth/[...nextauth]/
│   │   └── route.ts     # NextAuth API 라우트
│   └── login/
│       └── page.tsx     # 역할별 로그인 페이지
└── .env.local           # 환경 변수 (NEXTAUTH_URL 포트 구분)
```

### 인증 흐름 차이점

**web-main** (고객용):
- 모든 역할 로그인 가능
- 신규 유저: `MEMBER` 역할로 자동 생성

**web-admin** (관리자용):
- `ADMIN` 역할만 로그인 허용
- 미등록 유저: `ADMIN`으로 생성 (초기 세팅, 프로덕션에서 비활성화 필요)
- 비관리자 로그인 시 → `/login?error=unauthorized` 리다이렉트

**web-partner** (파트너용):
- `PARTNER` 역할만 로그인 허용
- 미등록 유저: `PARTNER`로 생성 (초기 세팅, 프로덕션에서 비활성화 필요)
- 비파트너 로그인 시 → `/login?error=unauthorized` 리다이렉트

### auth.ts 핵심 로직 (web-admin 예시)

```typescript
async signIn({ user, account }) {
  const existingUser = await prisma.user.findUnique({
    where: { email: user.email },
  });

  // 역할 체크: ADMIN이 아니면 거부
  if (existingUser && existingUser.role !== "ADMIN") {
    return "/login?error=unauthorized";
  }
  // ...
}
```

### Session에 role 포함 (전체 앱 공통)

```typescript
// JWT에 role 저장
async jwt({ token, account }) {
  if (token.email) {
    const dbUser = await prisma.user.findUnique({
      where: { email: token.email },
      select: { role: true },
    });
    token.role = dbUser?.role ?? undefined;
  }
  return token;
},

// Session에 role 전달
async session({ session, token }) {
  session.user.role = token.role;
  return session;
}
```

### Google Cloud Console 설정 (필수)

Google OAuth 콜백 URI 3개 추가 등록 필요:

```
http://localhost:3000/api/auth/callback/google   ← web-main (기존)
http://localhost:3001/api/auth/callback/google   ← web-admin (추가)
http://localhost:3002/api/auth/callback/google   ← web-partner (추가)
```

### .env.local 포트 설정

```env
# web-admin/.env.local
NEXTAUTH_URL=http://localhost:3001

# web-partner/.env.local
NEXTAUTH_URL=http://localhost:3002
```

### Prisma 클라이언트 생성

```bash
cd apps/web-admin && npx prisma generate
cd apps/web-partner && npx prisma generate
```

### 빌드 확인

```bash
pnpm --filter web-admin build    # ✅ 성공
pnpm --filter web-partner build  # ✅ 성공
```

---

### ❌ Google OAuth redirect_uri_mismatch 오류

**에러**
```
Error 400: redirect_uri_mismatch
The redirect URI in the request does not match the ones authorized for the OAuth client.
```

**원인**
Google Cloud Console의 OAuth 2.0 클라이언트에 web-admin (포트 3001),
web-partner (포트 3002)의 콜백 URL이 등록되지 않은 경우.

**해결**
Google Cloud Console → API 및 서비스 → 사용자 인증 정보 → OAuth 2.0 클라이언트에서
승인된 리디렉션 URI에 추가:
```
http://localhost:3001/api/auth/callback/google
http://localhost:3002/api/auth/callback/google
```

---

## 다음 단계 (Roadmap)

- [x] web-admin / web-partner 인증 흐름 적용
- [ ] 유저 정보 조회 API Route Handler (Mock API)
- [ ] GitHub Actions CI 파이프라인
- [ ] kind 로컬 K8s 첫 배포
- [ ] Helm Chart 작성
- [ ] ArgoCD GitOps 연동
- [ ] 멀티 테넌시 DB 전략
- [ ] Kafka 이벤트 스트림 연동

