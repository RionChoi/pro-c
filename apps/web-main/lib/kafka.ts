import { Kafka } from "kafkajs";

const globalForKafka = globalThis as unknown as {
  kafka: Kafka | undefined;
};

const brokers = (process.env.KAFKA_BROKERS ?? process.env.KAFKA_BROKER ?? "localhost:9092")
  .split(",")
  .map((b) => b.trim());

export const kafka =
  globalForKafka.kafka ??
  new Kafka({
    clientId: "web-main",
    brokers,
  });

if (process.env.NODE_ENV !== "production") globalForKafka.kafka = kafka;

export const producer = kafka.producer();

export function createConsumer(groupId = "web-main-group") {
  return kafka.consumer({ groupId });
}
