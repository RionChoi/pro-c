import { prisma } from "@/lib/prisma";

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

export async function createTenant(name: string, slug: string, plan = "FREE") {
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

  return tenant;
}

export function getPrismaForTenant(schemaName: string) {
  return prisma.$extends({
    query: {
      $allModels: {
        async $allOperations({ args, query }: { args: any; query: any }) {
          await prisma.$executeRawUnsafe(`SET search_path TO "${schemaName}"`);
          return query(args);
        },
      },
    },
  });
}
