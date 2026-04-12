import NextAuth from "next-auth";
import Google from "next-auth/providers/google";
import type { NextAuthConfig, Session } from "next-auth";
import type { JWT } from "next-auth/jwt";
import { prisma } from "@/lib/prisma";
import { producer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";

declare module "next-auth" {
  interface Session {
    accessToken?: string;
  }
}

declare module "next-auth/jwt" {
  interface JWT {
    accessToken?: string;
  }
}

export const config: NextAuthConfig = {
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
        const existing = await prisma.user.findUnique({
          where: { email: user.email },
        });

        if (!existing) {
          const newUser = await prisma.user.create({
            data: {
              email: user.email!,
              name: user.name,
              image: user.image,
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

          try {
            await producer.connect();
            await producer.send({
              topic: TOPICS.USER_CREATED,
              messages: [
                {
                  key: newUser.id,
                  value: JSON.stringify({
                    id: newUser.id,
                    email: newUser.email,
                    name: newUser.name,
                    role: newUser.role,
                    createdAt: newUser.createdAt,
                  }),
                },
              ],
            });
            await producer.disconnect();
          } catch (kafkaError) {
            console.error("Kafka 이벤트 발행 실패:", kafkaError);
          }
        }

        return true;
      } catch (e) {
        console.error(e);
        return false;
      }
    },
    async jwt({ token, account }): Promise<JWT> {
      if (account) token.accessToken = account.access_token ?? undefined;
      return token;
    },
    async session({ session, token }): Promise<Session> {
      session.accessToken = token.accessToken;
      return session;
    },
  },
  pages: { signIn: "/login" },
};

export const { handlers, auth, signIn, signOut } = NextAuth(config);
