"use server";

import { redirect } from "next/navigation";
import { auth } from "@/auth";
import { prisma } from "@repo/db";
import { createTenant } from "@/lib/tenant";

export async function createTenantAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const name = (formData.get("name") as string)?.trim();
  const slug = (formData.get("slug") as string)?.trim();

  if (!name || !slug) {
    redirect("/onboarding?error=required");
  }

  if (!/^[a-z0-9-]+$/.test(slug)) {
    redirect("/onboarding?error=invalid-slug");
  }

  const existing = await prisma.tenant.findUnique({ where: { slug } });
  if (existing) {
    redirect("/onboarding?error=slug-taken");
  }

  const user = await prisma.user.findUnique({ where: { email: session.user.email }, select: { id: true } });
  const tenant = await createTenant(
    name,
    slug,
    (formData.get("plan") as string | undefined) ?? "FREE",
    user?.id
  );

  await prisma.user.update({
    where: { email: session.user.email },
    data: { tenantId: tenant.id },
  });

  redirect("/dashboard");
}
