import { prisma } from "@repo/db";
import { producer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";
import { createDomainEvent } from "@repo/shared-types";

export async function createTenantSchema(schemaName: string) {
  await prisma.$executeRawUnsafe(`CREATE SCHEMA IF NOT EXISTS "${schemaName}"`);
  await prisma.$executeRawUnsafe(`
    CREATE TABLE IF NOT EXISTS "${schemaName}"."Project" (
      id TEXT PRIMARY KEY DEFAULT gen_random_uuid()::text,
      name TEXT NOT NULL,
      description TEXT,
      "createdAt" TIMESTAMP DEFAULT NOW(),
      "updatedAt" TIMESTAMP DEFAULT NOW()
    )
  `);
}

export async function createTenant(
  name: string,
  slug: string,
  plan = "FREE",
  creatorUserId?: string
) {
  const schemaName = `tenant_${slug.replace(/-/g, "_")}`;

  const tenant = await prisma.tenant.create({
    data: {
      name,
      slug,
      schemaName,
      plan: plan as "FREE" | "STARTER" | "PRO" | "ENTERPRISE",
      ...(creatorUserId && {
        members: { create: { userId: creatorUserId, role: "ADMIN" } },
        users:   { connect: { id: creatorUserId } },
      }),
    },
  });

  await createTenantSchema(schemaName);

  try {
    const event = createDomainEvent("TENANT_CREATED", tenant.id, {
      tenantId: tenant.id,
      name: tenant.name,
      slug: tenant.slug,
      plan: tenant.plan.toLowerCase() as "free" | "starter" | "pro" | "enterprise",
      adminEmail: "",
      createdAt: tenant.createdAt.toISOString(),
    });
    await producer.connect();
    await producer.send({
      topic: TOPICS.TENANT_CREATED,
      messages: [{ key: tenant.id, value: JSON.stringify(event) }],
    });
    await producer.disconnect();
  } catch (kafkaError) {
    console.error("Kafka 이벤트 발행 실패:", kafkaError);
  }

  return tenant;
}
