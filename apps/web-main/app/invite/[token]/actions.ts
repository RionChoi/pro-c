"use server";

import { redirect } from "next/navigation";
import { auth } from "@/auth";
import { prisma } from "@repo/db";

export async function acceptInviteAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const token = formData.get("token") as string;
  if (!token) redirect("/");

  const invite = await prisma.invite.findUnique({
    where: { token },
    select: { id: true, email: true, tenantId: true, role: true, usedAt: true, expiresAt: true },
  });

  if (!invite || invite.usedAt || invite.expiresAt < new Date()) {
    redirect("/dashboard?error=invite-invalid");
  }

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { id: true },
  });
  if (!user) redirect("/login");

  // 이미 멤버인 경우 초대만 소비하고 이동
  const existing = await prisma.tenantMember.findUnique({
    where: { userId_tenantId: { userId: user.id, tenantId: invite.tenantId } },
  });

  await prisma.$transaction([
    ...(existing ? [] : [
      prisma.tenantMember.create({
        data: { userId: user.id, tenantId: invite.tenantId, role: invite.role },
      }),
    ]),
    prisma.invite.update({
      where: { id: invite.id },
      data: { usedAt: new Date() },
    }),
    prisma.user.update({
      where: { id: user.id },
      data: { tenantId: invite.tenantId },
    }),
  ]);

  redirect("/dashboard");
}
