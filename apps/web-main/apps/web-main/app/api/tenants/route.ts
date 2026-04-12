import { NextRequest, NextResponse } from "next/server";
import { prisma } from "@/lib/prisma";
import { createTenant } from "@/lib/tenant";
import { auth } from "@/auth";

export async function GET() {
  const session = await auth();
  if (!session) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

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

export async function POST(req: NextRequest) {
  const session = await auth();
  if (!session) {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  const { name, slug, plan } = await req.json();

  if (!name || !slug) {
    return NextResponse.json(
      { error: "name and slug are required" },
      { status: 400 }
    );
  }

  const existing = await prisma.tenant.findUnique({ where: { slug } });
  if (existing) {
    return NextResponse.json(
      { error: "slug already exists" },
      { status: 409 }
    );
  }

  const tenant = await createTenant(name, slug, plan);

  return NextResponse.json({ data: tenant, success: true }, { status: 201 });
}
