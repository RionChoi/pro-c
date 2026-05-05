import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import Link from "next/link";

export default async function PartnerReportsPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    include: {
      memberships: {
        include: {
          tenant: {
            include: {
              invoices: {
                where: { status: "PAID" },
                orderBy: { paidAt: "desc" },
              },
              subscription: { select: { plan: true, status: true } },
            },
          },
        },
      },
    },
  });

  if (!user) redirect("/login");

  const tenants = user.memberships.map((m) => m.tenant);
  const allInvoices = tenants.flatMap((t) =>
    t.invoices.map((inv) => ({ ...inv, tenantName: t.name }))
  );
  allInvoices.sort((a, b) => {
    const dateA = a.paidAt?.getTime() ?? 0;
    const dateB = b.paidAt?.getTime() ?? 0;
    return dateB - dateA;
  });

  const totalRevenue = allInvoices.reduce((sum, inv) => sum + inv.amount, 0);
  const thisMonthInvoices = allInvoices.filter((inv) => {
    if (!inv.paidAt) return false;
    const now = new Date();
    return (
      inv.paidAt.getMonth() === now.getMonth() &&
      inv.paidAt.getFullYear() === now.getFullYear()
    );
  });
  const thisMonthRevenue = thisMonthInvoices.reduce((sum, inv) => sum + inv.amount, 0);

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
            <h1 className="text-2xl font-bold text-gray-900">매출 리포트</h1>
            <p className="mt-1 text-sm text-gray-500">관리 테넌트의 결제 내역을 확인합니다</p>
          </div>
          <Link
            href="/dashboard"
            className="rounded-lg border border-gray-300 px-4 py-2 text-sm font-medium text-gray-700 hover:bg-gray-50"
          >
            ← 대시보드
          </Link>
        </div>

        {/* Revenue Summary */}
        <div className="grid grid-cols-1 gap-4 sm:grid-cols-2 mb-10">
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">이번 달 매출</p>
            <p className="mt-1 text-3xl font-bold text-gray-900">
              {thisMonthRevenue.toLocaleString("ko-KR")}
              <span className="ml-1 text-base font-normal text-gray-400">원</span>
            </p>
            <p className="mt-1 text-xs text-gray-400">{thisMonthInvoices.length}건 결제</p>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">누적 매출</p>
            <p className="mt-1 text-3xl font-bold text-gray-900">
              {totalRevenue.toLocaleString("ko-KR")}
              <span className="ml-1 text-base font-normal text-gray-400">원</span>
            </p>
            <p className="mt-1 text-xs text-gray-400">총 {allInvoices.length}건</p>
          </div>
        </div>

        {/* Invoice History */}
        <div className="mb-4">
          <h2 className="text-lg font-semibold text-gray-900">결제 내역</h2>
        </div>

        <div className="rounded-xl border bg-white shadow-sm overflow-hidden">
          <table className="w-full text-sm">
            <thead className="border-b bg-gray-50">
              <tr>
                <th className="px-4 py-3 text-left font-medium text-gray-500">테넌트</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">플랜</th>
                <th className="px-4 py-3 text-right font-medium text-gray-500">금액</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">결제일</th>
              </tr>
            </thead>
            <tbody>
              {allInvoices.length === 0 ? (
                <tr>
                  <td colSpan={4} className="px-4 py-12 text-center text-gray-400">
                    결제 내역이 없습니다.
                  </td>
                </tr>
              ) : (
                allInvoices.slice(0, 20).map((inv) => (
                  <tr key={inv.id} className="border-b last:border-0 hover:bg-gray-50">
                    <td className="px-4 py-3 font-medium text-gray-900">{inv.tenantName}</td>
                    <td className="px-4 py-3">
                      <span className="inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-medium bg-blue-50 text-blue-700">
                        {inv.plan}
                      </span>
                    </td>
                    <td className="px-4 py-3 text-right font-mono text-gray-900">
                      {inv.amount.toLocaleString("ko-KR")}원
                    </td>
                    <td className="px-4 py-3 text-gray-500 text-xs">
                      {inv.paidAt?.toLocaleDateString("ko-KR") ?? "—"}
                    </td>
                  </tr>
                ))
              )}
            </tbody>
          </table>
        </div>
      </div>
    </main>
  );
}
