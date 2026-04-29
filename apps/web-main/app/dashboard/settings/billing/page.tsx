import { auth } from "@/auth";
import { prisma } from "@repo/db";
import { redirect } from "next/navigation";
import { PLAN_CONFIG } from "@repo/payments";
import type { Plan } from "@repo/db";
import BillingWidget from "./BillingWidget";

const PLAN_ORDER: Plan[] = ["FREE", "STARTER", "PRO", "ENTERPRISE"];

const STATUS_LABEL: Record<string, string> = {
  ACTIVE: "구독 중",
  PAST_DUE: "결제 실패 (유예)",
  CANCELED: "취소됨",
  EXPIRED: "만료됨",
};

export default async function BillingPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { tenantId: true, role: true },
  });

  if (!user?.tenantId) redirect("/onboarding");

  const [tenant, subscription, invoices] = await Promise.all([
    prisma.tenant.findUniqueOrThrow({
      where: { id: user.tenantId },
      select: { plan: true, tossBillingKey: true },
    }),
    prisma.subscription.findUnique({
      where: { tenantId: user.tenantId },
    }),
    prisma.invoice.findMany({
      where: { tenantId: user.tenantId },
      orderBy: { createdAt: "desc" },
      take: 10,
    }),
  ]);

  const isAdmin = user.role === "ADMIN";
  const hasCard = !!tenant.tossBillingKey;
  const currentConfig = PLAN_CONFIG[tenant.plan];

  return (
    <main className="min-h-screen bg-gray-50">
      <div className="mx-auto max-w-3xl px-6 py-10 space-y-8">
        <div>
          <h1 className="text-2xl font-bold text-gray-900">구독 & 결제</h1>
          <p className="mt-1 text-sm text-gray-500">플랜 변경 및 결제 내역을 관리합니다.</p>
        </div>

        {/* 현재 플랜 */}
        <section className="rounded-xl border bg-white p-6 shadow-sm">
          <div className="flex items-start justify-between">
            <div>
              <p className="text-sm text-gray-500">현재 플랜</p>
              <p className="mt-1 text-2xl font-bold text-gray-900">{currentConfig.name}</p>
              <p className="mt-1 text-sm text-gray-500">
                {currentConfig.priceKrw === 0
                  ? "무료"
                  : `₩${currentConfig.priceKrw.toLocaleString()} / 월`}
              </p>
            </div>
            {subscription && (
              <span className={`rounded-full px-3 py-1 text-xs font-medium ${
                subscription.status === "ACTIVE"
                  ? "bg-green-50 text-green-700"
                  : "bg-yellow-50 text-yellow-700"
              }`}>
                {STATUS_LABEL[subscription.status] ?? subscription.status}
              </span>
            )}
          </div>
          {subscription && (
            <p className="mt-3 text-xs text-gray-400">
              다음 갱신일: {subscription.currentPeriodEnd.toLocaleDateString("ko-KR")}
              {subscription.cancelAtPeriodEnd && " (기간 만료 후 해지)"}
            </p>
          )}
        </section>

        {/* 플랜 선택 */}
        {isAdmin && (
          <section className="rounded-xl border bg-white p-6 shadow-sm">
            <h2 className="text-base font-semibold text-gray-900 mb-4">플랜 변경</h2>
            <div className="grid grid-cols-2 gap-3 sm:grid-cols-4">
              {PLAN_ORDER.map((plan) => {
                const config = PLAN_CONFIG[plan];
                const isCurrent = plan === tenant.plan;
                return (
                  <div
                    key={plan}
                    className={`rounded-lg border p-4 ${
                      isCurrent
                        ? "border-blue-500 bg-blue-50"
                        : "border-gray-200 bg-white"
                    }`}
                  >
                    <p className="font-semibold text-sm text-gray-900">{config.name}</p>
                    <p className="mt-1 text-xs text-gray-500">
                      {config.priceKrw === 0
                        ? "무료"
                        : `₩${config.priceKrw.toLocaleString()}`}
                    </p>
                    <p className="mt-2 text-xs text-gray-400">
                      멤버 {config.maxMembers === -1 ? "무제한" : `${config.maxMembers}명`}
                    </p>
                    {isCurrent && (
                      <span className="mt-2 inline-block text-xs font-medium text-blue-600">
                        현재
                      </span>
                    )}
                  </div>
                );
              })}
            </div>

            <div className="mt-6">
              <BillingWidget
                currentPlan={tenant.plan}
                hasCard={hasCard}
                clientKey={process.env.NEXT_PUBLIC_TOSS_CLIENT_KEY ?? ""}
              />
            </div>
          </section>
        )}

        {/* 결제 내역 */}
        <section className="rounded-xl border bg-white shadow-sm">
          <div className="px-6 py-4 border-b">
            <h2 className="text-base font-semibold text-gray-900">결제 내역</h2>
          </div>
          {invoices.length === 0 ? (
            <p className="px-6 py-8 text-sm text-gray-400 text-center">결제 내역이 없습니다.</p>
          ) : (
            <table className="w-full text-sm">
              <thead>
                <tr className="border-b bg-gray-50 text-left text-xs text-gray-500">
                  <th className="px-6 py-3">날짜</th>
                  <th className="px-6 py-3">플랜</th>
                  <th className="px-6 py-3">금액</th>
                  <th className="px-6 py-3">상태</th>
                </tr>
              </thead>
              <tbody className="divide-y">
                {invoices.map((inv) => (
                  <tr key={inv.id} className="hover:bg-gray-50">
                    <td className="px-6 py-3 text-gray-600">
                      {inv.createdAt.toLocaleDateString("ko-KR")}
                    </td>
                    <td className="px-6 py-3 text-gray-900">
                      {PLAN_CONFIG[inv.plan].name}
                    </td>
                    <td className="px-6 py-3 text-gray-900">
                      ₩{inv.amount.toLocaleString()}
                    </td>
                    <td className="px-6 py-3">
                      <span className={`rounded-full px-2 py-0.5 text-xs font-medium ${
                        inv.status === "PAID"
                          ? "bg-green-50 text-green-700"
                          : inv.status === "FAILED"
                          ? "bg-red-50 text-red-700"
                          : "bg-gray-100 text-gray-600"
                      }`}>
                        {inv.status === "PAID" ? "완료" : inv.status === "FAILED" ? "실패" : inv.status}
                      </span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </section>
      </div>
    </main>
  );
}
