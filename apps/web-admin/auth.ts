import { NextAuth, createAuthConfig } from "@repo/auth";

export const { handlers, auth, signIn, signOut, unstable_update } = NextAuth(
  createAuthConfig({ allowedRole: "ADMIN", defaultRole: "ADMIN" })
);
