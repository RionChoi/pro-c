import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import Link from "next/link";

export default async function PartnerTenantsPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    include: {
      memberships: {
        include: {
          tenant: {
            include: {
              _count: { select: { users: true, members: true, invites: true } },
              subscription: true,
              invoices: {
                where: { status: "PAID" },
                orderBy: { paidAt: "desc" },
                take: 1,
              },
            },
          },
        },
      },
    },
  });

  if (!user) redirect("/login");

  const tenants = user.memberships.map((m) => m.tenant);

  return (
    <main className="min-h-screen bg-gray-50">
      <nav className="border-b bg-white px-6 py-4 flex items-center justify-between">
        <div className="flex items-center gap-3">
          <Link href="/dashboard" className="flex items-center gap-3 hover:opacity-80">
            <div className="flex h-8 w-8 items-center justify-center rounded-lg bg-green-600 text-white text-sm font-bold">
              P
            </div>
            <span className="font-semibold text-gray-900">Partner Portal</span>
          </Link>
        </div>
        <div className="flex items-center gap-4">
          <span className="text-sm text-gray-500">
            {session.user.name ?? session.user.email}
          </span>
          <form
            action={async () => {
              "use server";
              const { signOut } = await import("@/auth");
              await signOut({ redirectTo: "/login" });
            }}
          >
            <button type="submit" className="text-sm text-gray-500 hover:text-gray-900">
              로그아웃
            </button>
          </form>
        </div>
      </nav>

      <div className="mx-auto max-w-6xl px-6 py-10">
        <div className="mb-8 flex items-center justify-between">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">테넌트 상세</h1>
            <p className="mt-1 text-sm text-gray-500">관리 중인 테넌트의 상세 정보</p>
          </div>
          <Link
            href="/dashboard"
            className="rounded-lg border border-gray-300 px-4 py-2 text-sm font-medium text-gray-700 hover:bg-gray-50"
          >
            ← 대시보드
          </Link>
        </div>

        {tenants.length === 0 ? (
          <div className="rounded-xl border bg-white p-12 text-center shadow-sm">
            <p className="text-gray-400">관리 중인 테넌트가 없습니다.</p>
          </div>
        ) : (
          <div className="grid grid-cols-1 gap-6">
            {tenants.map((tenant) => (
              <div key={tenant.id} className="rounded-xl border bg-white shadow-sm overflow-hidden">
                <div className="border-b bg-gray-50 px-6 py-4 flex items-center justify-between">
                  <div>
                    <h3 className="font-semibold text-gray-900">{tenant.name}</h3>
                    <p className="text-xs text-gray-400 font-mono">{tenant.slug}</p>
                  </div>
                  <span className={`inline-flex items-center rounded-full px-3 py-1 text-xs font-medium ${
                    tenant.plan === "ENTERPRISE" ? "bg-amber-50 text-amber-700" :
                    tenant.plan === "PRO" ? "bg-purple-50 text-purple-700" :
                    tenant.plan === "STARTER" ? "bg-blue-50 text-blue-700" :
                    "bg-gray-100 text-gray-600"
                  }`}>
                    {tenant.plan}
                  </span>
                </div>

                <div className="px-6 py-5">
                  <div className="grid grid-cols-2 gap-6 sm:grid-cols-4">
                    <div>
                      <p className="text-xs text-gray-500 mb-1">사용자 수</p>
                      <p className="text-lg font-semibold text-gray-900">{tenant._count.users}명</p>
                    </div>
                    <div>
                      <p className="text-xs text-gray-500 mb-1">멤버 수</p>
                      <p className="text-lg font-semibold text-gray-900">{tenant._count.members}명</p>
                    </div>
                    <div>
                      <p className="text-xs text-gray-500 mb-1">대기 초대</p>
                      <p className="text-lg font-semibold text-gray-900">{tenant._count.invites}건</p>
                    </div>
                    <div>
                      <p className="text-xs text-gray-500 mb-1">구독 상태</p>
                      <p className="text-lg font-semibold text-gray-900">
                        {tenant.subscription ? (
                          <span className={
                            tenant.subscription.status === "ACTIVE" ? "text-green-600" :
                            tenant.subscription.status === "PAST_DUE" ? "text-yellow-600" :
                            "text-gray-500"
                          }>
                            {tenant.subscription.status === "ACTIVE" ? "활성" :
                             tenant.subscription.status === "PAST_DUE" ? "지연" :
                             tenant.subscription.status === "CANCELED" ? "해지" : "만료"}
                          </span>
                        ) : (
                          <span className="text-gray-400">미구독</span>
                        )}
                      </p>
                    </div>
                  </div>

                  {tenant.subscription && (
                    <div className="mt-4 rounded-lg bg-gray-50 px-4 py-3">
                      <div className="flex items-center justify-between text-sm">
                        <span className="text-gray-500">구독 기간</span>
                        <span className="text-gray-700">
                          {tenant.subscription.currentPeriodStart.toLocaleDateString("ko-KR")}
                          {" ~ "}
                          {tenant.subscription.currentPeriodEnd.toLocaleDateString("ko-KR")}
                        </span>
                      </div>
                      {tenant.subscription.cancelAtPeriodEnd && (
                        <p className="mt-2 text-xs text-red-500">
                          ⚠️ 현재 구독 기간 종료 후 자동 해지 예정
                        </p>
                      )}
                    </div>
                  )}

                  {tenant.invoices.length > 0 && (
                    <div className="mt-3 text-xs text-gray-400">
                      최근 결제: {tenant.invoices[0]!.paidAt?.toLocaleDateString("ko-KR")} ·{" "}
                      {(tenant.invoices[0]!.amount).toLocaleString("ko-KR")}원
                    </div>
                  )}
                </div>
              </div>
            ))}
          </div>
        )}
      </div>
    </main>
  );
}
