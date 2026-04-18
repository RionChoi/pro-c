import { NextAuth, createAuthConfig } from "@repo/auth";

export const { handlers, auth, signIn, signOut } = NextAuth(
  createAuthConfig({ allowedRole: "PARTNER", defaultRole: "PARTNER" })
);
