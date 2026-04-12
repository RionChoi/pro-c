import { Kafka } from "kafkajs";

const globalForKafka = globalThis as unknown as {
  kafka: Kafka | undefined;
};

export const kafka =
  globalForKafka.kafka ??
  new Kafka({
    clientId: "web-main",
    brokers: [process.env.KAFKA_BROKER ?? "localhost:9092"],
  });

if (process.env.NODE_ENV !== "production") globalForKafka.kafka = kafka;

export const producer = kafka.producer();
export const consumer = kafka.consumer({ groupId: "web-main-group" });
