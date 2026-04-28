import { NextAuth, createAuthConfig } from "@repo/auth";
import { producer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";
import { createDomainEvent } from "@repo/shared-types";

export const { handlers, auth, signIn, signOut } = NextAuth(
  createAuthConfig({
    onNewUser: async (user) => {
      try {
        const event = createDomainEvent("USER_CREATED", "", {
          userId: user.id,
          email: user.email,
          name: user.name ?? "",
          role: "member" as const,
          tenantId: "",
          createdAt: user.createdAt.toISOString(),
        });
        await producer.connect();
        await producer.send({
          topic: TOPICS.USER_CREATED,
          messages: [{ key: user.id, value: JSON.stringify(event) }],
        });
        await producer.disconnect();
      } catch (kafkaError) {
        console.error("Kafka 이벤트 발행 실패:", kafkaError);
      }
    },
  })
);
