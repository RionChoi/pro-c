import { NextAuth, createAuthConfig } from "@repo/auth";
import { producer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";

export const { handlers, auth, signIn, signOut } = NextAuth(
  createAuthConfig({
    onNewUser: async (user) => {
      try {
        await producer.connect();
        await producer.send({
          topic: TOPICS.USER_CREATED,
          messages: [{ key: user.id, value: JSON.stringify(user) }],
        });
        await producer.disconnect();
      } catch (kafkaError) {
        console.error("Kafka 이벤트 발행 실패:", kafkaError);
      }
    },
  })
);
