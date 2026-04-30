import { prisma } from "@repo/db";
import { producer } from "@/lib/kafka";
import { TOPICS } from "@/lib/kafka-topics";
import { createDomainEvent } from "@repo/shared-types";

const SCHEMA_IDENTIFIER_REGEX = /^tenant_[a-z0-9_]+$/;
const MAX_POSTGRES_IDENTIFIER_LENGTH = 63;

function buildTenantSchemaName(slug: string): string {
  const normalizedSlug = slug.trim().toLowerCase().replace(/-/g, "_");

  if (!/^[a-z0-9_]+$/.test(normalizedSlug)) {
    throw new Error("Invalid tenant slug: only lowercase letters, numbers, and '-' are allowed.");
  }

  const schemaName = `tenant_${normalizedSlug}`;

  if (!SCHEMA_IDENTIFIER_REGEX.test(schemaName)) {
    throw new Error("Invalid schema identifier generated from tenant slug.");
  }

  if (schemaName.length > MAX_POSTGRES_IDENTIFIER_LENGTH) {
    throw new Error("Schema name too long. Keep slug shorter.");
  }

  return schemaName;
}

export async function createTenantSchema(schemaName: string) {
  if (!SCHEMA_IDENTIFIER_REGEX.test(schemaName)) {
    throw new Error("Unsafe schema identifier blocked.");
  }

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
  const schemaName = buildTenantSchemaName(slug);

  const tenant = await prisma.$transaction(async (tx) => {
    if (!SCHEMA_IDENTIFIER_REGEX.test(schemaName)) {
      throw new Error("Unsafe schema identifier blocked.");
    }

    await tx.$executeRawUnsafe(`CREATE SCHEMA IF NOT EXISTS "${schemaName}"`);
    await tx.$executeRawUnsafe(`
      CREATE TABLE IF NOT EXISTS "${schemaName}"."Project" (
        id TEXT PRIMARY KEY DEFAULT gen_random_uuid()::text,
        name TEXT NOT NULL,
        description TEXT,
        "createdAt" TIMESTAMP DEFAULT NOW(),
        "updatedAt" TIMESTAMP DEFAULT NOW()
      )
    `);

    return tx.tenant.create({
      data: {
        name,
        slug,
        schemaName,
        plan: plan as "FREE" | "STARTER" | "PRO" | "ENTERPRISE",
        ...(creatorUserId && {
          members: { create: { userId: creatorUserId, role: "ADMIN" } },
          users: { connect: { id: creatorUserId } },
        }),
      },
    });
  });

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
