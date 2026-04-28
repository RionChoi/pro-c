import { NextResponse } from "next/server";
import { prisma } from "@repo/db";
import { requireRole } from "@/lib/with-role";

export async function GET() {
  const check = await requireRole();
  if (!check.ok) return check.response;

  const users = await prisma.user.findMany({
    select: {
      id: true,
      email: true,
      name: true,
      role: true,
      createdAt: true,
    },
    orderBy: { createdAt: "desc" },
  });

  return NextResponse.json({ data: users, success: true });
}
