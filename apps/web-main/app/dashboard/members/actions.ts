"use server";

import { redirect } from "next/navigation";
import { auth } from "@/auth";
import { prisma } from "@repo/db";
import type { Role } from "@repo/auth";

export async function inviteMemberAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const inviter = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { id: true, tenantId: true, role: true },
  });

  if (!inviter?.tenantId || inviter.role !== "ADMIN") {
    redirect("/dashboard/members?error=forbidden");
  }

  const email = (formData.get("email") as string)?.trim().toLowerCase();
  const role = (formData.get("role") as Role) ?? "MEMBER";

  if (!email) redirect("/dashboard/members?error=invalid");

  // 이미 멤버인지 확인
  const existingUser = await prisma.user.findUnique({ where: { email }, select: { id: true } });
  if (existingUser) {
    const existing = await prisma.tenantMember.findUnique({
      where: { userId_tenantId: { userId: existingUser.id, tenantId: inviter.tenantId } },
    });
    if (existing) redirect("/dashboard/members?error=already-member");
  }

  // 기존 유효 초대 재활용 방지 — 만료되지 않은 초대가 있으면 삭제 후 재생성
  await prisma.invite.deleteMany({
    where: { email, tenantId: inviter.tenantId, usedAt: null },
  });

  const expiresAt = new Date(Date.now() + 7 * 24 * 60 * 60 * 1000); // 7일

  const invite = await prisma.invite.create({
    data: { email, tenantId: inviter.tenantId, role, expiresAt },
    include: { tenant: { select: { name: true } } },
  });

  const inviteUrl = `${process.env.NEXTAUTH_URL}/invite/${invite.token}`;

  // TODO: Resend / SMTP로 실제 이메일 발송 연동
  console.log(`[invite] ${email} → ${inviteUrl} (tenant: ${invite.tenant.name}, role: ${role})`);

  redirect("/dashboard/members?success=1");
}
