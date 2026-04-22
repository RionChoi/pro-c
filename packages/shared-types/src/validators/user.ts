/**
 * User Validators — Zod Runtime Validation Schemas
 *
 * These schemas provide runtime validation for API inputs.
 * They complement TypeScript types by validating data at runtime.
 * Inferred types match the input shapes.
 */

import { z } from "zod";

// ============================================================
// Role & Enum Schemas
// ============================================================

export const UserRoleSchema = z.enum(["admin", "member", "partner", "viewer"]);

export const PlanTypeSchema = z.enum(["free", "starter", "pro", "enterprise"]);

// ============================================================
// User Schemas
// ============================================================

/**
 * Schema for creating a new user.
 * @precondition email must be a valid email format
 * @precondition name must be 2-100 characters
 * @precondition role must be one of: admin, member, partner, viewer
 */
export const CreateUserSchema = z.object({
  email: z.string().email("Invalid email format"),
  name: z
    .string()
    .min(2, "Name must be at least 2 characters")
    .max(100, "Name must be at most 100 characters"),
  role: UserRoleSchema,
  tenantId: z.string().uuid("Invalid tenant ID format"),
});

/**
 * Schema for updating an existing user.
 * All fields are optional — only provided fields are validated.
 */
export const UpdateUserSchema = z.object({
  name: z
    .string()
    .min(2, "Name must be at least 2 characters")
    .max(100, "Name must be at most 100 characters")
    .optional(),
  role: UserRoleSchema.optional(),
});

/**
 * Schema for user search/filtering.
 */
export const UserQuerySchema = z.object({
  page: z.coerce.number().int().positive().default(1),
  limit: z.coerce.number().int().min(1).max(100).default(20),
  role: UserRoleSchema.optional(),
  search: z.string().max(100).optional(),
});

// ============================================================
// Organization/Tenant Schemas
// ============================================================

/**
 * Schema for creating a new tenant/organization.
 */
export const CreateTenantSchema = z.object({
  name: z
    .string()
    .min(2, "Name must be at least 2 characters")
    .max(100, "Name must be at most 100 characters"),
  slug: z
    .string()
    .min(2, "Slug must be at least 2 characters")
    .max(50, "Slug must be at most 50 characters")
    .regex(
      /^[a-z0-9-]+$/,
      "Slug must contain only lowercase letters, numbers, and hyphens"
    ),
  plan: PlanTypeSchema.default("free"),
  adminEmail: z.string().email("Invalid admin email format"),
});

/**
 * Schema for updating a tenant.
 */
export const UpdateTenantSchema = z.object({
  name: z
    .string()
    .min(2, "Name must be at least 2 characters")
    .max(100, "Name must be at most 100 characters")
    .optional(),
  slug: z
    .string()
    .min(2, "Slug must be at least 2 characters")
    .max(50, "Slug must be at most 50 characters")
    .regex(
      /^[a-z0-9-]+$/,
      "Slug must contain only lowercase letters, numbers, and hyphens"
    )
    .optional(),
});

/**
 * Schema for changing tenant plan.
 */
export const ChangeTenantPlanSchema = z.object({
  tenantId: z.string().uuid("Invalid tenant ID format"),
  plan: PlanTypeSchema,
});

// ============================================================
// Inferred Types (from schemas)
// ============================================================

export type CreateUserInput = z.infer<typeof CreateUserSchema>;
export type UpdateUserInput = z.infer<typeof UpdateUserSchema>;
export type UserQueryInput = z.infer<typeof UserQuerySchema>;
export type CreateTenantInput = z.infer<typeof CreateTenantSchema>;
export type UpdateTenantInput = z.infer<typeof UpdateTenantSchema>;
export type ChangeTenantPlanInput = z.infer<typeof ChangeTenantPlanSchema>;

// ============================================================
// Validation Helpers
// ============================================================

/**
 * Safely parses and validates data against a schema.
 * @returns { success: true, data } or { success: false, error }
 * @example
 * const result = safeParse(CreateUserSchema, { email: "bad", name: "A" });
 * if (!result.success) {
 *   console.log(result.error.flatten());
 * }
 */
export function safeParse<T extends z.ZodType>(
  schema: T,
  data: unknown
): { success: true; data: z.infer<T> } | { success: false; error: z.ZodError } {
  const result = schema.safeParse(data);
  if (result.success) {
    return { success: true, data: result.data };
  }
  return { success: false, error: result.error };
}

/**
 * Validates data and throws if invalid.
 * Use in trusted environments where you want to fail fast.
 */
export function validate<T extends z.ZodType>(
  schema: T,
  data: unknown,
  context?: string
): asserts data is z.infer<T> {
  const result = schema.safeParse(data);
  if (!result.success) {
    const message = context
      ? `Validation failed for ${context}: ${result.error.message}`
      : `Validation failed: ${result.error.message}`;
    throw new Error(message);
  }
}
