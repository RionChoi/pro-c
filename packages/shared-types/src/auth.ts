export interface TokenPayload {
  sub: string;
  email: string;
  role: UserRole;
  tenantId: string;
  iat: number;
  exp: number;
}

export interface Session {
  user: AuthUser;
  accessToken: string;
  expiresAt: number;
}

export interface AuthUser {
  id: string;
  email: string;
  name: string;
  role: UserRole;
  tenantId: string;
}

export type UserRole = "admin" | "member" | "partner" | "viewer";
