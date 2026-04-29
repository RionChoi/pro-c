import { NextResponse } from "next/server";
import { prisma } from "@repo/db";
import { requireRole } from "@repo/auth";
import { auth } from "@/auth";

export async function GET() {
  const session = await auth();
  const check = requireRole(session, "ADMIN");
  if (!check.ok) return check.response;

  const tenants = await prisma.tenant.findMany({
    select: {
      id: true,
      name: true,
      slug: true,
      plan: true,
      schemaName: true,
      createdAt: true,
      _count: { select: { users: true } },
    },
    orderBy: { createdAt: "desc" },
  });

  return NextResponse.json({ data: tenants, success: true });
}
