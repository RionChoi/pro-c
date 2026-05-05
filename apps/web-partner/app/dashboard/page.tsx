import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import Link from "next/link";

export default async function PartnerDashboardPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    include: {
      memberships: {
        include: {
          tenant: {
            include: {
              _count: { select: { users: true, members: true } },
              subscription: { select: { plan: true, status: true, currentPeriodEnd: true } },
            },
          },
        },
      },
    },
  });

  if (!user) redirect("/login");

  const tenants = user.memberships.map((m) => m.tenant);
  const totalTenants = tenants.length;
  const activePlans = tenants.filter(
    (t) => t.subscription && t.subscription.status === "ACTIVE"
  ).length;
  const totalUsers = tenants.reduce((sum, t) => sum + t._count.users, 0);

  return (
    <main className="min-h-screen bg-gray-50">
      {/* Navigation */}
      <nav className="border-b bg-white px-6 py-4 flex items-center justify-between">
        <div className="flex items-center gap-3">
          <div className="flex h-8 w-8 items-center justify-center rounded-lg bg-green-600 text-white text-sm font-bold">
            P
          </div>
          <span className="font-semibold text-gray-900">Partner Portal</span>
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
            <button
              type="submit"
              className="text-sm text-gray-500 hover:text-gray-900"
            >
              로그아웃
            </button>
          </form>
        </div>
      </nav>

      <div className="mx-auto max-w-6xl px-6 py-10">
        {/* Header */}
        <div className="mb-8">
          <h1 className="text-2xl font-bold text-gray-900">파트너 대시보드</h1>
          <p className="mt-1 text-sm text-gray-500">
            관리 중인 테넌트의 현황을 한눈에 확인하세요
          </p>
        </div>

        {/* Summary Cards */}
        <div className="grid grid-cols-1 gap-4 sm:grid-cols-3 mb-10">
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <div className="flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-green-50">
                <svg className="h-5 w-5 text-green-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 21V5a2 2 0 00-2-2H7a2 2 0 00-2 2v16m14 0h2m-2 0h-5m-9 0H3m2 0h5M9 7h1m-1 4h1m4-4h1m-1 4h1m-5 10v-5a1 1 0 011-1h2a1 1 0 011 1v5m-4 0h4" />
                </svg>
              </div>
              <div>
                <p className="text-sm text-gray-500">관리 테넌트</p>
                <p className="text-2xl font-bold text-gray-900">{totalTenants}개</p>
              </div>
            </div>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <div className="flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-blue-50">
                <svg className="h-5 w-5 text-blue-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
                </svg>
              </div>
              <div>
                <p className="text-sm text-gray-500">활성 구독</p>
                <p className="text-2xl font-bold text-gray-900">{activePlans}개</p>
              </div>
            </div>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <div className="flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-purple-50">
                <svg className="h-5 w-5 text-purple-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M17 20h5v-2a3 3 0 00-5.356-1.857M17 20H7m10 0v-2c0-.656-.126-1.283-.356-1.857M7 20H2v-2a3 3 0 015.356-1.857M7 20v-2c0-.656.126-1.283.356-1.857m0 0a5.002 5.002 0 019.288 0M15 7a3 3 0 11-6 0 3 3 0 016 0z" />
                </svg>
              </div>
              <div>
                <p className="text-sm text-gray-500">총 사용자</p>
                <p className="text-2xl font-bold text-gray-900">{totalUsers}명</p>
              </div>
            </div>
          </div>
        </div>

        {/* Tenant List */}
        <div className="mb-4">
          <h2 className="text-lg font-semibold text-gray-900">관리 테넌트 현황</h2>
        </div>

        <div className="rounded-xl border bg-white shadow-sm overflow-hidden">
          <table className="w-full text-sm">
            <thead className="border-b bg-gray-50">
              <tr>
                <th className="px-4 py-3 text-left font-medium text-gray-500">테넌트</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">플랜</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">구독 상태</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">사용자</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">다음 결제일</th>
              </tr>
            </thead>
            <tbody>
              {tenants.length === 0 ? (
                <tr>
                  <td colSpan={5} className="px-4 py-12 text-center text-gray-400">
                    <p className="text-base">관리 중인 테넌트가 없습니다.</p>
                    <p className="mt-1 text-xs">관리자에게 파트너 멤버십을 요청하세요.</p>
                  </td>
                </tr>
              ) : (
                tenants.map((tenant) => (
                  <tr key={tenant.id} className="border-b last:border-0 hover:bg-gray-50">
                    <td className="px-4 py-3">
                      <div>
                        <p className="font-medium text-gray-900">{tenant.name}</p>
                        <p className="text-xs text-gray-400 font-mono">{tenant.slug}</p>
                      </div>
                    </td>
                    <td className="px-4 py-3">
                      <PlanBadge plan={tenant.plan} />
                    </td>
                    <td className="px-4 py-3">
                      <StatusBadge status={tenant.subscription?.status} />
                    </td>
                    <td className="px-4 py-3 text-gray-500">
                      {tenant._count.users}명
                    </td>
                    <td className="px-4 py-3 text-gray-500 text-xs">
                      {tenant.subscription?.currentPeriodEnd
                        ? tenant.subscription.currentPeriodEnd.toLocaleDateString("ko-KR")
                        : "—"}
                    </td>
                  </tr>
                ))
              )}
            </tbody>
          </table>
        </div>

        {/* Quick Links */}
        <div className="mt-8 grid grid-cols-1 gap-4 sm:grid-cols-2">
          <Link
            href="/tenants"
            className="rounded-xl border bg-white p-5 shadow-sm hover:border-green-300 hover:shadow-md transition-all group"
          >
            <div className="flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-green-50 group-hover:bg-green-100 transition-colors">
                <svg className="h-5 w-5 text-green-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5H7a2 2 0 00-2 2v12a2 2 0 002 2h10a2 2 0 002-2V7a2 2 0 00-2-2h-2M9 5a2 2 0 002 2h2a2 2 0 002-2M9 5a2 2 0 012-2h2a2 2 0 012 2" />
                </svg>
              </div>
              <div>
                <p className="font-medium text-gray-900">테넌트 상세</p>
                <p className="text-xs text-gray-500">개별 테넌트의 상세 정보를 확인합니다</p>
              </div>
            </div>
          </Link>
          <Link
            href="/reports"
            className="rounded-xl border bg-white p-5 shadow-sm hover:border-blue-300 hover:shadow-md transition-all group"
          >
            <div className="flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-blue-50 group-hover:bg-blue-100 transition-colors">
                <svg className="h-5 w-5 text-blue-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
                </svg>
              </div>
              <div>
                <p className="font-medium text-gray-900">리포트</p>
                <p className="text-xs text-gray-500">매출 및 성과 리포트를 확인합니다</p>
              </div>
            </div>
          </Link>
        </div>
      </div>
    </main>
  );
}

function PlanBadge({ plan }: { plan: string }) {
  const styles: Record<string, string> = {
    FREE: "bg-gray-100 text-gray-600",
    STARTER: "bg-blue-50 text-blue-700",
    PRO: "bg-purple-50 text-purple-700",
    ENTERPRISE: "bg-amber-50 text-amber-700",
  };

  return (
    <span className={`inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-medium ${styles[plan] ?? styles.FREE}`}>
      {plan}
    </span>
  );
}

function StatusBadge({ status }: { status?: string | null }) {
  if (!status) {
    return <span className="text-xs text-gray-400">미구독</span>;
  }

  const styles: Record<string, string> = {
    ACTIVE: "bg-green-50 text-green-700",
    PAST_DUE: "bg-yellow-50 text-yellow-700",
    CANCELED: "bg-red-50 text-red-700",
    EXPIRED: "bg-gray-100 text-gray-600",
  };

  const labels: Record<string, string> = {
    ACTIVE: "활성",
    PAST_DUE: "결제 지연",
    CANCELED: "해지됨",
    EXPIRED: "만료",
  };

  return (
    <span className={`inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-medium ${styles[status] ?? "bg-gray-100 text-gray-600"}`}>
      {labels[status] ?? status}
    </span>
  );
}
