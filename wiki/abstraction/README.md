# Abstraction Wiki — Platform

> 이 폴더는 Platform 프로젝트의 Abstraction(추상화) 원칙과 현재 구조를 문서화합니다.
> 작성일: 2026-04-22

---

## 1. 현재 Abstraction 구조 (Current State)

Platform은 `packages/shared-types/`를 통해 **공유 계약(Contract)**을 중앙화하고 있습니다.

```
packages/shared-types/src/
├── index.ts      # Exports all
├── api.ts        # API 공통 응답/에러 계약
├── auth.ts       # 인증/인가 계약
└── user.ts       # 사용자/조직 계약
```

### 1.1 API 계약 (`api.ts`)

| 계약 | 목적 |
|---|---|
| `ApiResponse<T>` | 단일 리소스 응답 |
| `PaginatedResponse<T>` | 목록/페이지네이션 응답 |
| `ApiError` | 에러 응답 구조 |

```typescript
export interface ApiResponse<T> {
  data: T;
  message?: string;
  success: boolean;
}
```

### 1.2 인증 계약 (`auth.ts`)

| 계약 | 목적 |
|---|---|
| `TokenPayload` | JWT 내용 |
| `Session` | 사용자 세션 |
| `AuthUser` | 인증된 사용자 정보 |
| `UserRole` | 역할 타입 (`admin` \| `member` \| `partner` \| `viewer`) |

```typescript
export interface TokenPayload {
  sub: string;
  email: string;
  role: UserRole;
  tenantId: string;
  iat: number;
  exp: number;
}
```

### 1.3 사용자/조직 계약 (`user.ts`)

| 계약 | 목적 |
|---|---|
| `User` | 플랫폼 사용자 |
| `Organization` | 테넌트/조직 |
| `Permission` | 리소스별 권한 |
| `PlanType` | 플랜 타입 (`free` \| `starter` \| `pro` \| `enterprise`) |

```typescript
export interface User {
  id: string;
  email: string;
  name: string;
  role: UserRole;
  tenantId: string;
  createdAt: string;
  updatedAt: string;
}
```

---

## 2. 현재 사용하는 Abstraction 패턴

| 패턴 | 적용 위치 | 설명 |
|---|---|---|
| **Interface Segregation** | `shared-types/*` | 필요한 타입만 import |
| **Type Re-export** | `user.ts` → `UserRole` re-export | 중복 정의 방지 |
| **Generic Response** | `ApiResponse<T>` | 제네릭으로 타입 안전성 확보 |
| **Discriminated Union** | `UserRole`, `PlanType` | 유한 상태 표현 |

---

## 3. 현재 구조의 강점

- ✅ **단일 소스 (Single Source of Truth)**: 모든 앱이 동일한 계약 사용
- ✅ **타입 안전성**: TypeScript strict mode와 결합
- ✅ **간단한 의존성**: `@repo/shared-types` 하나만 import
- ✅ **개념 분리**: API / Auth / User로 도메인 분리

---

## 4. 현재 구조의 개선 필요 영역

자세한 개선안은 `IMPROVEMENTS.md`를 참고하세요.

요약:
1. **커맨드/쿼리 분리 (CQRS)** — API 계약 확장 필요성
2. **이벤트 계약 부재** — Kafka 메시지용 Event 스키마 없음
3. **검증 스키마 부재** — Zod/Valibot 같은 runtime validation 없음
4. **OpenAPI Integration** — API 계약 → OpenAPI 문서 자동 생성 미비

---

## 5. 관련 파일

- [IMPROVEMENTS.md](./IMPROVEMENTS.md) — 추상화 강화 추천안
- `packages/shared-types/src/` — 실제 구현
- `packages/auth/src/` — NextAuth 설정
- `packages/db/src/` — Prisma schema

---

## 6. 추상화 원칙 (Abstraction Principles)

### 6.1 인터페이스 분리 원칙 (ISP)
> 클라이언트가 사용하지 않는 메서드에 의존하지 않아야 한다.

```typescript
// ❌ Bad: 모든 필드를 한 번에
interface User { id, email, name, role, tenantId, createdAt, updatedAt, profile, settings, ... }

// ✅ Good: 필요한 만큼만
interface UserSummary { id, name, avatar }
interface UserProfile { id, email, bio, avatar }
```

### 6.2 의존성 역전 원칙 (DIP)
> 고수준 모듈은 저수준 모듈에 의존하지 않아야 한다. 둘 다 추상화에 의존해야 한다.

```typescript
// ❌ Bad: 구체 구현에 직접 의존
import { PrismaUserRepository } from "@repo/db"

// ✅ Good: 추상화 인터페이스에 의존
import type { IUserRepository } from "@repo/shared-types"
```

### 6.3 계약에 의한 설계 (DbC)
> 인터페이스에 사전조건/사후조건을 문서화

```typescript
/**
 * @precondition - email must be valid format
 * @precondition - role must be one of: admin, member, partner, viewer
 * @postcondition - returns User with generated id and timestamps
 */
interface CreateUserInput { email: string; name: string; role: UserRole }
```
