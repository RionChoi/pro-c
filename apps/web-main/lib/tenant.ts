import { prisma } from "@/lib/prisma";
import { producer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";

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
  plan = "FREE"
) {
  const schemaName = `tenant_${slug.replace(/-/g, "_")}`;

  const tenant = await prisma.tenant.create({
    data: {
      name,
      slug,
      schemaName,
      plan: plan as "FREE" | "STARTER" | "PRO" | "ENTERPRISE",
    },
  });

  await createTenantSchema(schemaName);

  try {
    await producer.connect();
    await producer.send({
      topic: TOPICS.TENANT_CREATED,
      messages: [
        {
          key: tenant.id,
          value: JSON.stringify({
            id: tenant.id,
            name: tenant.name,
            slug: tenant.slug,
            plan: tenant.plan,
            schemaName: tenant.schemaName,
            createdAt: tenant.createdAt,
          }),
        },
      ],
    });
    await producer.disconnect();
  } catch (kafkaError) {
    console.error("Kafka 이벤트 발행 실패:", kafkaError);
  }

  return tenant;
}
