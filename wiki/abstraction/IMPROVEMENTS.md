# Abstraction Improvements — Platform

> 현재 구조를 강화하기 위한 구체적인 개선안.
> 작성일: 2026-04-22

---

## 1. 이벤트 계약 추가 (Event Schema)

**문제**: Kafka 사용하지만 이벤트 계약이 없어서 타입 안전성 없음.

**개선안**:

```typescript
// packages/shared-types/src/events.ts

export type EventType =
  | "TENANT_CREATED"
  | "TENANT_UPDATED"
  | "USER_JOINED"
  | "USER_LEFT"
  | "SUBSCRIPTION_CHANGED";

export interface DomainEvent<T = unknown> {
  id: string;
  type: EventType;
  tenantId: string;
  payload: T;
  metadata: {
    timestamp: string;
    version: number;
    correlationId?: string;
  };
}

// 예시 이벤트
export interface TenantCreatedEvent {
  tenantId: string;
  name: string;
  plan: PlanType;
  adminEmail: string;
}

export interface TenantCreatedPayload {
  tenant: TenantCreatedEvent;
}
```

**파일 추가**: `packages/shared-types/src/events.ts`

---

## 2. 커맨드/쿼리 계약 분리 (CQRS Contracts)

**문제**: 현재 `ApiResponse<T>`가 명령과 쿼리를 구분하지 않음.

**개선안**:

```typescript
// packages/shared-types/src/cqrs.ts

// Query Contract — 읽기 전용
export interface QueryResult<T> {
  data: T;
  pagination?: {
    total: number;
    page: number;
    limit: number;
    hasNext: boolean;
  };
  timestamp: string;
}

// Command Contract — 쓰기 전용
export interface CommandResult<T = void> {
  success: boolean;
  data?: T;
  error?: ApiError;
  affectedCount?: number;
}

// Eventual Consistency Marker
export interface AsyncCommandResult {
  jobId: string;
  status: "pending" | "processing" | "completed" | "failed";
  estimatedCompletion?: string;
}
```

**파일 추가**: `packages/shared-types/src/cqrs.ts`

---

## 3. 검증 스키마 추가 (Runtime Validation)

**문제**: TypeScript는 런타임에 타입이 사라지므로 API 입력 검증이 없음.

**개선안** (Zod 사용):

```typescript
// packages/shared-types/src/validators/user.ts
import { z } from "zod";

export const UserRoleSchema = z.enum(["admin", "member", "partner", "viewer"]);

export const CreateUserSchema = z.object({
  email: z.string().email(),
  name: z.string().min(2).max(100),
  role: UserRoleSchema,
});

export const UpdateUserSchema = z.object({
  name: z.string().min(2).max(100).optional(),
  role: UserRoleSchema.optional(),
});

// Inference
export type CreateUserInput = z.infer<typeof CreateUserSchema>;
```

**파일 추가**: `packages/shared-types/src/validators/user.ts`

---

## 4. OpenAPI 통합

**문제**: API 문서가手動管理로 유지보수困难.

**개선안**: `@repo/shared-types`의 계약 → OpenAPI 스키마 자동 생성.

```typescript
// packages/shared-types/src/openapi.ts

/**
 * @openapi
 * components:
 *   schemas:
 *     User:
 *       type: object
 *       properties:
 *         id:
 *           type: string
 *         email:
 *           type: string
 *         name:
 *           type: string
 */
export interface User { ... }
```

**도구 활용**: `tsoa`, `swagger-jsdoc`, `@anatine/zod-openapi`

---

## 5. 의존성 역전 (Repository Pattern)

**문제**: 앱들이 직접 `PrismaClient`를 사용하면 구현 변경 시 파급.

**개선안**: 추상화 인터페이스 + 구현 분리.

```typescript
// packages/shared-types/src/repositories.ts

export interface IUserRepository {
  findById(id: string): Promise<User | null>;
  findByEmail(email: string): Promise<User | null>;
  create(data: CreateUserInput): Promise<User>;
  update(id: string, data: Partial<UpdateUserInput>): Promise<User>;
  delete(id: string): Promise<void>;
}

export interface ITenantRepository {
  findById(id: string): Promise<Tenant | null>;
  create(data: CreateTenantInput): Promise<Tenant>;
  updatePlan(id: string, plan: PlanType): Promise<Tenant>;
}
```

**구현 분리**:
```
packages/
├── shared-types/src/repositories.ts  ← 추상화
├── db/src/user.repository.ts         ← Prisma 구현
apps/web-main/lib/user.repository.ts  ← 앱별 구현 (필요 시)
```

---

## 6. 우선순위 매트릭스

| 개선안 | 복잡성 | 영향도 | 우선순위 |
|---|---|---|---|
| 1. 이벤트 계약 추가 | 낮음 | 높음 | 🔴 높음 |
| 2. CQRS 계약 분리 | 중간 | 중간 | 🟡 중간 |
| 3. 검증 스키마 추가 | 낮음 | 높음 | 🔴 높음 |
| 4. OpenAPI 통합 | 높음 | 중간 | 🟡 중간 |
| 5. Repository 패턴 | 높음 | 높음 | 🟡 중간 (ADR 필요) |

---

## 7. 즉시 적용 가능한 것 (Quick Wins)

### 7.1 파일 추가 (1시간 이내)
- `events.ts` — Kafka 이벤트 계약
- `validators/user.ts` — Zod 스키마

### 7.2 문서화 (ADR 작성 필요)
- `repository.ts` — Repository 패턴 도입 여부 (ADR)

### 7.3 장기 과제
- OpenAPI 통합
- 전체 Prisma → Repository 추상화

---

## 8. 다음 액션

| 담당 | 액션 | 마일스톤 |
|---|---|---|
| Hermes | `events.ts` 생성 | 오늘 |
| Hermes | `validators/` 폴더 + Zod 스키마 | 오늘 |
| Claude Code | ADR-008: Repository 패턴 도입 검토 | 이번 주 |
| Ekko | 개선안 리뷰 및 승인 | 이번 주 |
