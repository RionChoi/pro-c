/**
 * Domain Events — Kafka Message Contracts
 *
 * These schemas define the structure of events published to the message bus.
 * Used by producers (apps) and consumers (services) for type-safe event handling.
 */

import type { PlanType, UserRole } from "./user";

// ============================================================
// Core Event Structure
// ============================================================

export interface EventMetadata {
  timestamp: string; // ISO 8601
  version: number;   // Schema version for migration
  correlationId?: string; // For tracing across services
}

export interface DomainEvent<T = unknown> {
  id: string;        // UUID v4
  type: EventType;
  tenantId: string;
  payload: T;
  metadata: EventMetadata;
}

// ============================================================
// Event Types
// ============================================================

export type EventType =
  // Tenant Events
  | "TENANT_CREATED"
  | "TENANT_UPDATED"
  | "TENANT_DELETED"
  | "TENANT_PLAN_CHANGED"
  // User Events
  | "USER_CREATED"
  | "USER_UPDATED"
  | "USER_DELETED"
  | "USER_ROLE_CHANGED"
  | "USER_JOINED_TENANT"
  | "USER_LEFT_TENANT"
  // Subscription Events
  | "SUBSCRIPTION_STARTED"
  | "SUBSCRIPTION_CANCELLED"
  | "SUBSCRIPTION_RENEWED";

// ============================================================
// Tenant Events
// ============================================================

export interface TenantCreatedPayload {
  tenantId: string;
  name: string;
  slug: string;
  plan: PlanType;
  adminEmail: string;
  createdAt: string;
}

export interface TenantUpdatedPayload {
  tenantId: string;
  name?: string;
  slug?: string;
}

export interface TenantDeletedPayload {
  tenantId: string;
  deletedAt: string;
}

export interface TenantPlanChangedPayload {
  tenantId: string;
  previousPlan: PlanType;
  newPlan: PlanType;
  changedAt: string;
}

// ============================================================
// User Events
// ============================================================

export interface UserCreatedPayload {
  userId: string;
  email: string;
  name: string;
  role: UserRole;
  tenantId: string;
  createdAt: string;
}

export interface UserUpdatedPayload {
  userId: string;
  name?: string;
  email?: string;
}

export interface UserDeletedPayload {
  userId: string;
  tenantId: string;
  deletedAt: string;
}

export interface UserRoleChangedPayload {
  userId: string;
  tenantId: string;
  previousRole: UserRole;
  newRole: UserRole;
  changedAt: string;
}

export interface UserJoinedTenantPayload {
  userId: string;
  tenantId: string;
  role: UserRole;
  joinedAt: string;
}

export interface UserLeftTenantPayload {
  userId: string;
  tenantId: string;
  leftAt: string;
}

// ============================================================
// Subscription Events
// ============================================================

export interface SubscriptionStartedPayload {
  tenantId: string;
  plan: PlanType;
  startDate: string;
  endDate: string;
}

export interface SubscriptionCancelledPayload {
  tenantId: string;
  plan: PlanType;
  cancelledAt: string;
  effectiveEndDate: string;
}

export interface SubscriptionRenewedPayload {
  tenantId: string;
  plan: PlanType;
  previousEndDate: string;
  newEndDate: string;
}

// ============================================================
// Type Guards
// ============================================================

export function isTenantEvent(type: string): boolean {
  return type.startsWith("TENANT_");
}

export function isUserEvent(type: string): boolean {
  return type.startsWith("USER_");
}

export function isSubscriptionEvent(type: string): boolean {
  return type.startsWith("SUBSCRIPTION_");
}

// ============================================================
// Factory Helpers
// ============================================================

/**
 * Creates a DomainEvent with auto-generated metadata.
 * @example
 * const event = createTenantCreatedEvent({
 *   tenantId: "123",
 *   name: "Acme Corp",
 *   ...
 * });
 */
export function createDomainEvent<T>(
  type: EventType,
  tenantId: string,
  payload: T,
  correlationId?: string
): DomainEvent<T> {
  return {
    id: crypto.randomUUID(),
    type,
    tenantId,
    payload,
    metadata: {
      timestamp: new Date().toISOString(),
      version: 1,
      correlationId,
    },
  };
}

// Re-export for convenience
export { type PlanType, type UserRole };
