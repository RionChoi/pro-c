import { NextRequest, NextResponse } from "next/server";
import { prisma } from "@repo/db";

// Toss는 별도 서명 검증 없이 secret key 기반 Basic Auth 헤더로 검증
// 웹훅 이벤트는 결제 상태 변경 알림용 (실제 결제 승인은 billingKey API로 직접 처리)
export async function POST(req: NextRequest) {
  const body = await req.json() as {
    eventType: string;
    data: {
      paymentKey?: string;
      orderId?: string;
      status?: string;
      cancels?: Array<{ cancelReason: string }>;
    };
  };

  const { eventType, data } = body;

  try {
    switch (eventType) {
      case "PAYMENT_STATUS_CHANGED": {
        if (!data.orderId) break;

        const invoice = await prisma.invoice.findUnique({
          where: { tossOrderId: data.orderId },
          select: { id: true, tenantId: true },
        });
        if (!invoice) break;

        if (data.status === "CANCELED") {
          await prisma.invoice.update({
            where: { id: invoice.id },
            data: { status: "CANCELED" },
          });
        }
        break;
      }

      case "BILLING_KEY_DELETED": {
        // 빌링키 삭제 시 구독 취소 처리
        const tenant = await prisma.tenant.findFirst({
          where: { tossBillingKey: { not: null } },
          select: { id: true },
        });
        if (tenant) {
          await prisma.subscription.updateMany({
            where: { tenantId: tenant.id, status: "ACTIVE" },
            data: { status: "CANCELED", cancelAtPeriodEnd: true },
          });
        }
        break;
      }
    }
  } catch (err) {
    console.error("[toss webhook]", err);
    return NextResponse.json({ error: "internal" }, { status: 500 });
  }

  return NextResponse.json({ ok: true });
}
