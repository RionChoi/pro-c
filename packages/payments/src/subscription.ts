import { prisma } from "@repo/db";
import type { Plan } from "@repo/db";
import { getPlanConfig } from "./plans";
import { chargeBilling } from "./toss";
import { randomUUID } from "crypto";

// 구독 생성 (최초 빌링키 등록 + 첫 결제)
export async function createSubscription(tenantId: string, plan: Plan): Promise<void> {
  const tenant = await prisma.tenant.findUniqueOrThrow({
    where: { id: tenantId },
    select: { tossBillingKey: true, tossCustomerKey: true, name: true },
  });

  if (!tenant.tossBillingKey || !tenant.tossCustomerKey) {
    throw new Error("빌링키가 등록되지 않았습니다.");
  }

  const config = getPlanConfig(plan);
  const orderId = `order_${tenantId}_${Date.now()}`;
  const now = new Date();
  const periodEnd = new Date(now);
  periodEnd.setMonth(periodEnd.getMonth() + 1);

  const invoice = await prisma.invoice.create({
    data: {
      tenantId,
      plan,
      amount: config.priceKrw,
      tossOrderId: orderId,
      status: "PENDING",
    },
  });

  try {
    const payment = await chargeBilling(tenant.tossBillingKey, {
      customerKey: tenant.tossCustomerKey,
      amount: config.priceKrw,
      orderId,
      orderName: `${config.name} 플랜 1개월`,
      customerEmail: "",
    });

    await prisma.$transaction([
      prisma.invoice.update({
        where: { id: invoice.id },
        data: {
          status: "PAID",
          tossPaymentKey: payment.paymentKey,
          paidAt: new Date(payment.approvedAt),
        },
      }),
      prisma.subscription.upsert({
        where: { tenantId },
        create: {
          tenantId,
          plan,
          status: "ACTIVE",
          currentPeriodStart: now,
          currentPeriodEnd: periodEnd,
        },
        update: {
          plan,
          status: "ACTIVE",
          currentPeriodStart: now,
          currentPeriodEnd: periodEnd,
          cancelAtPeriodEnd: false,
        },
      }),
      prisma.tenant.update({
        where: { id: tenantId },
        data: { plan },
      }),
    ]);
  } catch (err) {
    await prisma.invoice.update({
      where: { id: invoice.id },
      data: {
        status: "FAILED",
        failReason: err instanceof Error ? err.message : "결제 실패",
      },
    });
    throw err;
  }
}

// 구독 취소 (기간 만료 시 해지)
export async function cancelSubscription(tenantId: string): Promise<void> {
  await prisma.subscription.update({
    where: { tenantId },
    data: { cancelAtPeriodEnd: true },
  });
}

// 플랜 한도 초과 여부 확인
export async function checkPlanLimit(
  tenantId: string,
  resource: "members" | "projects"
): Promise<{ allowed: boolean; limit: number; current: number }> {
  const tenant = await prisma.tenant.findUniqueOrThrow({
    where: { id: tenantId },
    select: {
      plan: true,
      _count: { select: { members: true } },
    },
  });

  const config = getPlanConfig(tenant.plan);
  const limit = resource === "members" ? config.maxMembers : config.maxProjects;
  const current = resource === "members" ? tenant._count.members : 0;

  return {
    allowed: limit === -1 || current < limit,
    limit,
    current,
  };
}

export { randomUUID };
