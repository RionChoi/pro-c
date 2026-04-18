import NextAuth from "next-auth";
import Google from "next-auth/providers/google";
import type { NextAuthConfig, Session } from "next-auth";
import { prisma } from "@repo/db";
import type { Role } from "@repo/db";

declare module "next-auth" {
  interface Session {
    accessToken?: string;
    user: {
      id?: string;
      role?: string;
      email?: string | null;
      name?: string | null;
      image?: string | null;
    };
  }
}

export type NewUserPayload = {
  id: string;
  email: string;
  name: string | null;
  role: Role;
  createdAt: Date;
};

export type CreateAuthConfigOptions = {
  allowedRole?: Role;
  defaultRole?: Role;
  onNewUser?: (user: NewUserPayload) => Promise<void>;
};

type ExtendedToken = {
  accessToken?: string;
  role?: string;
  email?: string | null;
  [key: string]: unknown;
};

export function createAuthConfig(options: CreateAuthConfigOptions = {}): NextAuthConfig {
  const { allowedRole, defaultRole = "MEMBER", onNewUser } = options;

  return {
    providers: [
      Google({
        clientId: process.env.AUTH_GOOGLE_ID,
        clientSecret: process.env.AUTH_GOOGLE_SECRET,
      }),
    ],
    callbacks: {
      async signIn({ user, account }) {
        if (!user.email) return false;
        try {
          const existingUser = await prisma.user.findUnique({
            where: { email: user.email },
          });

          if (existingUser) {
            if (allowedRole && existingUser.role !== allowedRole) {
              console.warn(`[auth] 접근 거부: ${user.email} (role: ${existingUser.role})`);
              return "/login?error=unauthorized";
            }
            return true;
          }

          const newUser = await prisma.user.create({
            data: {
              email: user.email,
              name: user.name,
              image: user.image,
              role: allowedRole ?? defaultRole,
              accounts: {
                create: {
                  provider: account!.provider,
                  providerAccountId: account!.providerAccountId,
                  access_token: account!.access_token,
                  refresh_token: account!.refresh_token,
                  expires_at: account!.expires_at,
                },
              },
            },
          });

          await onNewUser?.({
            id: newUser.id,
            email: newUser.email,
            name: newUser.name,
            role: newUser.role,
            createdAt: newUser.createdAt,
          });

          return true;
        } catch (error) {
          console.error("[auth] signIn 오류:", error);
          return false;
        }
      },
      async jwt({ token, account }) {
        const t = token as ExtendedToken;
        if (account) t.accessToken = account.access_token ?? undefined;
        if (t.email) {
          const dbUser = await prisma.user.findUnique({
            where: { email: t.email },
            select: { role: true },
          });
          t.role = dbUser?.role ?? undefined;
        }
        return t;
      },
      async session({ session, token }): Promise<Session> {
        const t = token as ExtendedToken;
        session.accessToken = t.accessToken;
        session.user.role = t.role;
        return session;
      },
    },
    pages: {
      signIn: "/login",
      error: "/login",
    },
  };
}

export { NextAuth };
export type { NextAuthConfig };
