/**
 * Event Validators — Zod Runtime Validation for Domain Events
 *
 * These schemas validate incoming Kafka messages and API event payloads.
 */

import { z } from "zod";
import type { EventType } from "../events";

// ============================================================
// Event Metadata
// ============================================================

export const EventMetadataSchema = z.object({
  timestamp: z.string().datetime({ message: "Invalid ISO 8601 timestamp" }),
  version: z.number().int().positive().default(1),
  correlationId: z.string().uuid().optional(),
});

// ============================================================
// Event Type Enum
// ============================================================

export const EventTypeSchema = z.enum([
  "TENANT_CREATED",
  "TENANT_UPDATED",
  "TENANT_DELETED",
  "TENANT_PLAN_CHANGED",
  "USER_CREATED",
  "USER_UPDATED",
  "USER_DELETED",
  "USER_ROLE_CHANGED",
  "USER_JOINED_TENANT",
  "USER_LEFT_TENANT",
  "SUBSCRIPTION_STARTED",
  "SUBSCRIPTION_CANCELLED",
  "SUBSCRIPTION_RENEWED",
]);

// ============================================================
// Tenant Event Payloads
// ============================================================

export const TenantCreatedPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  name: z.string().min(2).max(100),
  slug: z.string().min(2).max(50),
  plan: z.enum(["free", "starter", "pro", "enterprise"]),
  adminEmail: z.string().email(),
  createdAt: z.string().datetime(),
});

export const TenantUpdatedPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  name: z.string().min(2).max(100).optional(),
  slug: z.string().min(2).max(50).optional(),
});

export const TenantDeletedPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  deletedAt: z.string().datetime(),
});

export const TenantPlanChangedPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  previousPlan: z.enum(["free", "starter", "pro", "enterprise"]),
  newPlan: z.enum(["free", "starter", "pro", "enterprise"]),
  changedAt: z.string().datetime(),
});

// ============================================================
// User Event Payloads
// ============================================================

export const UserCreatedPayloadSchema = z.object({
  userId: z.string().uuid(),
  email: z.string().email(),
  name: z.string().min(2).max(100),
  role: z.enum(["admin", "member", "partner", "viewer"]),
  tenantId: z.string().uuid(),
  createdAt: z.string().datetime(),
});

export const UserUpdatedPayloadSchema = z.object({
  userId: z.string().uuid(),
  name: z.string().min(2).max(100).optional(),
  email: z.string().email().optional(),
});

export const UserDeletedPayloadSchema = z.object({
  userId: z.string().uuid(),
  tenantId: z.string().uuid(),
  deletedAt: z.string().datetime(),
});

export const UserRoleChangedPayloadSchema = z.object({
  userId: z.string().uuid(),
  tenantId: z.string().uuid(),
  previousRole: z.enum(["admin", "member", "partner", "viewer"]),
  newRole: z.enum(["admin", "member", "partner", "viewer"]),
  changedAt: z.string().datetime(),
});

export const UserJoinedTenantPayloadSchema = z.object({
  userId: z.string().uuid(),
  tenantId: z.string().uuid(),
  role: z.enum(["admin", "member", "partner", "viewer"]),
  joinedAt: z.string().datetime(),
});

export const UserLeftTenantPayloadSchema = z.object({
  userId: z.string().uuid(),
  tenantId: z.string().uuid(),
  leftAt: z.string().datetime(),
});

// ============================================================
// Subscription Event Payloads
// ============================================================

export const SubscriptionStartedPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  plan: z.enum(["free", "starter", "pro", "enterprise"]),
  startDate: z.string().datetime(),
  endDate: z.string().datetime(),
});

export const SubscriptionCancelledPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  plan: z.enum(["free", "starter", "pro", "enterprise"]),
  cancelledAt: z.string().datetime(),
  effectiveEndDate: z.string().datetime(),
});

export const SubscriptionRenewedPayloadSchema = z.object({
  tenantId: z.string().uuid(),
  plan: z.enum(["free", "starter", "pro", "enterprise"]),
  previousEndDate: z.string().datetime(),
  newEndDate: z.string().datetime(),
});

// ============================================================
// Generic Event Schemas
// ============================================================

export const DomainEventSchema = z.object({
  id: z.string().uuid(),
  type: EventTypeSchema,
  tenantId: z.string().uuid(),
  payload: z.record(z.unknown()), // Validated per-type separately
  metadata: EventMetadataSchema,
});

// ============================================================
// Union Schema for All Event Payloads
// ============================================================

export const EventPayloadSchema = z.union([
  TenantCreatedPayloadSchema,
  TenantUpdatedPayloadSchema,
  TenantDeletedPayloadSchema,
  TenantPlanChangedPayloadSchema,
  UserCreatedPayloadSchema,
  UserUpdatedPayloadSchema,
  UserDeletedPayloadSchema,
  UserRoleChangedPayloadSchema,
  UserJoinedTenantPayloadSchema,
  UserLeftTenantPayloadSchema,
  SubscriptionStartedPayloadSchema,
  SubscriptionCancelledPayloadSchema,
  SubscriptionRenewedPayloadSchema,
]);

// ============================================================
// Type Exports
// ============================================================

export type CreateTenantEventInput = z.infer<typeof TenantCreatedPayloadSchema>;
export type CreateUserEventInput = z.infer<typeof UserCreatedPayloadSchema>;
export type ChangeRoleEventInput = z.infer<typeof UserRoleChangedPayloadSchema>;
export type ChangePlanEventInput = z.infer<typeof TenantPlanChangedPayloadSchema>;
