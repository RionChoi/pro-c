export interface User {
  id: string;
  email: string;
  name: string;
  role: UserRole;
  tenantId: string;
  createdAt: string;
  updatedAt: string;
}

export interface Organization {
  id: string;
  name: string;
  slug: string;
  plan: PlanType;
  createdAt: string;
}

export interface Permission {
  resource: string;
  action: "create" | "read" | "update" | "delete";
}

export type UserRole = "admin" | "member" | "partner" | "viewer";
export type PlanType = "free" | "starter" | "pro" | "enterprise";
