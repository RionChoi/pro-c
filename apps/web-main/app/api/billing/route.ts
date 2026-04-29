import { NextRequest, NextResponse } from "next/server";
import { prisma } from "@repo/db";
import { requireRole } from "@repo/auth";
import { issueBillingKey, createSubscription } from "@repo/payments";
import { auth } from "@/auth";
import { randomUUID } from "crypto";
import type { Plan } from "@repo/db";

const VALID_PLANS: Plan[] = ["STARTER", "PRO", "ENTERPRISE"];

// POST /api/billing — 빌링키 등록 + 구독 시작
export async function POST(req: NextRequest) {
  const session = await auth();
  const check = requireRole(session, "ADMIN");
  if (!check.ok) return check.response;

  const body = await req.json() as { authKey?: string; plan?: string };
  const { authKey, plan } = body;

  if (!authKey || !plan || !VALID_PLANS.includes(plan as Plan)) {
    return NextResponse.json({ error: "authKey and valid plan are required" }, { status: 400 });
  }

  const user = await prisma.user.findUniqueOrThrow({
    where: { email: session!.user.email! },
    select: { tenantId: true },
  });

  if (!user.tenantId) {
    return NextResponse.json({ error: "테넌트가 없습니다." }, { status: 400 });
  }

  const customerKey = randomUUID();

  const billing = await issueBillingKey(authKey, customerKey);

  await prisma.tenant.update({
    where: { id: user.tenantId },
    data: {
      tossBillingKey: billing.billingKey,
      tossCustomerKey: customerKey,
    },
  });

  await createSubscription(user.tenantId, plan as Plan);

  return NextResponse.json({ ok: true });
}

// DELETE /api/billing — 구독 취소
export async function DELETE() {
  const session = await auth();
  const check = requireRole(session, "ADMIN");
  if (!check.ok) return check.response;

  const user = await prisma.user.findUniqueOrThrow({
    where: { email: session!.user.email! },
    select: { tenantId: true },
  });

  if (!user.tenantId) {
    return NextResponse.json({ error: "테넌트가 없습니다." }, { status: 400 });
  }

  await prisma.subscription.update({
    where: { tenantId: user.tenantId },
    data: { cancelAtPeriodEnd: true },
  });

  return NextResponse.json({ ok: true });
}
