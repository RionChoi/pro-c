import { NextRequest, NextResponse } from "next/server";
import { prisma } from "@repo/db";
import { requireRole } from "@/lib/with-role";
import type { Role } from "@repo/auth";

const VALID_ROLES: Role[] = ["ADMIN", "MEMBER", "PARTNER", "VIEWER"];

export async function PATCH(
  req: NextRequest,
  { params }: { params: Promise<{ id: string }> }
) {
  const check = await requireRole("ADMIN");
  if (!check.ok) return check.response;

  const { id } = await params;
  const body = await req.json() as { role?: unknown };

  if (!body.role || !VALID_ROLES.includes(body.role as Role)) {
    return NextResponse.json({ error: "Invalid role" }, { status: 400 });
  }

  const updated = await prisma.user.update({
    where: { id },
    data: { role: body.role as Role },
    select: { id: true, email: true, name: true, role: true },
  });

  return NextResponse.json({ data: updated, success: true });
}
