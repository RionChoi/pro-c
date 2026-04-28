"use server";

import { redirect } from "next/navigation";
import { auth } from "@/auth";
import { prisma } from "@repo/db";

export async function selectTenantAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const tenantId = formData.get("tenantId") as string;
  if (!tenantId) redirect("/tenants");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { id: true },
  });
  if (!user) redirect("/login");

  // 해당 테넌트의 실제 멤버인지 검증
  const membership = await prisma.tenantMember.findUnique({
    where: { userId_tenantId: { userId: user.id, tenantId } },
  });
  if (!membership) redirect("/tenants");

  await prisma.user.update({
    where: { id: user.id },
    data: { tenantId },
  });

  redirect("/dashboard");
}
