export async function register() {
  // Edge runtime에서는 KafkaJS 동작 불가 — Node.js 전용
  if (process.env.NEXT_RUNTIME !== "nodejs") return;

  // Kafka 브로커가 설정되지 않은 환경(테스트, 로컬 등)에서는 skip
  if (!process.env.KAFKA_BROKERS && !process.env.KAFKA_BROKER) return;

  const { startKafkaConsumer } = await import("@/lib/kafka-consumer");
  try {
    await startKafkaConsumer();
  } catch {
    // non-fatal: Kafka unavailable in this environment
  }
}
