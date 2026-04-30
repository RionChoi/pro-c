import type { Consumer } from "kafkajs";
import type {
  DomainEvent,
  TenantCreatedPayload,
  UserCreatedPayload,
  UserRoleChangedPayload,
} from "@repo/shared-types";
import { createConsumer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";
import { sendMail } from "@/lib/mail";

let consumerInstance: Consumer | null = null;

async function handleTenantCreated(event: DomainEvent<TenantCreatedPayload>) {
  const { tenantId, name, slug, plan, adminEmail } = event.payload;
  console.log(`[kafka] tenant.created — ${slug} (${tenantId}) plan=${plan} name=${name}`);
  
  // 테넌트 생성 알림 (어드민에게)
  await sendMail({
    to: adminEmail,
    subject: `Welcome to Platform! Your tenant ${name} is ready.`,
    html: `<h1>Welcome!</h1><p>Your tenant <strong>${name}</strong> has been successfully created on the ${plan} plan.</p>`,
  });
}

async function handleUserCreated(event: DomainEvent<UserCreatedPayload>) {
  const { userId, email, role, tenantId, name } = event.payload;
  console.log(`[kafka] user.created — ${email} (${userId}) role=${role} tenant=${tenantId}`);
  
  // 웰컴 이메일 발송
  await sendMail({
    to: email,
    subject: `Welcome to Platform, ${name}!`,
    html: `<h1>Welcome!</h1><p>Your account has been created on Platform. We're glad to have you!</p>`,
  });
}

async function handleUserRoleChanged(event: DomainEvent<UserRoleChangedPayload>) {
  const { userId, tenantId, previousRole, newRole } = event.payload;
  console.log(
    `[kafka] user.role.changed — ${userId} tenant=${tenantId} ${previousRole} → ${newRole}`
  );
  // TODO: 권한 캐시 무효화, 감사 로그 기록
}

function parseMessage<T>(value: Buffer | string | null): DomainEvent<T> | null {
  if (!value) return null;
  try {
    return JSON.parse(value.toString()) as DomainEvent<T>;
  } catch {
    return null;
  }
}

export async function startKafkaConsumer(): Promise<void> {
  if (consumerInstance) return;

  const consumer = createConsumer("web-main-group");
  consumerInstance = consumer;

  try {
    await consumer.connect();
    await consumer.subscribe({
      topics: [TOPICS.TENANT_CREATED, TOPICS.USER_CREATED, TOPICS.USER_ROLE_CHANGED],
      fromBeginning: false,
    });

    await consumer.run({
      eachMessage: async ({ topic, message }) => {
        switch (topic) {
          case TOPICS.TENANT_CREATED: {
            const event = parseMessage<TenantCreatedPayload>(message.value);
            if (event) await handleTenantCreated(event);
            break;
          }
          case TOPICS.USER_CREATED: {
            const event = parseMessage<UserCreatedPayload>(message.value);
            if (event) await handleUserCreated(event);
            break;
          }
          case TOPICS.USER_ROLE_CHANGED: {
            const event = parseMessage<UserRoleChangedPayload>(message.value);
            if (event) await handleUserRoleChanged(event);
            break;
          }
        }
      },
    });

    console.log("[kafka] consumer started");
  } catch (err) {
    console.warn("[kafka] consumer start failed — running without Kafka:", (err as Error).message);
    consumerInstance = null;
  }
}

export async function stopKafkaConsumer(): Promise<void> {
  if (!consumerInstance) return;
  await consumerInstance.disconnect();
  consumerInstance = null;
  console.log("[kafka] consumer stopped");
}
