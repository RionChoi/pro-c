import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import Link from "next/link";

export default async function DashboardPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    include: { tenant: true },
  });

  if (!user) redirect("/login");
  if (!user.tenant) redirect("/onboarding");

  const { tenant } = user;

  const memberCount = await prisma.tenantMember.count({ where: { tenantId: tenant.id } });

  return (
    <main className="min-h-screen bg-gray-50">
      <nav className="border-b bg-white px-6 py-4 flex items-center justify-between">
        <div className="flex items-center gap-4">
          <span className="font-semibold text-gray-900">{tenant.name}</span>
          <Link
            href="/tenants"
            className="text-xs text-gray-400 hover:text-gray-600 underline"
          >
            워크스페이스 전환
          </Link>
        </div>
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
      </nav>

      <div className="mx-auto max-w-5xl px-6 py-10">
        <div className="mb-8">
          <h1 className="text-2xl font-bold text-gray-900">대시보드</h1>
          <p className="mt-1 text-sm text-gray-500">
            {session.user.name ?? session.user.email} · {user.role}
          </p>
        </div>

        <div className="grid grid-cols-1 gap-4 sm:grid-cols-4 mb-10">
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">플랜</p>
            <p className="mt-1 text-2xl font-bold text-gray-900">{tenant.plan}</p>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">슬러그</p>
            <p className="mt-1 text-2xl font-bold text-gray-900">{tenant.slug}</p>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">멤버</p>
            <p className="mt-1 text-2xl font-bold text-gray-900">{memberCount}명</p>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">생성일</p>
            <p className="mt-1 text-xl font-bold text-gray-900">
              {tenant.createdAt.toLocaleDateString("ko-KR")}
            </p>
          </div>
        </div>

        <div className="flex items-center justify-between mb-4">
          <h2 className="text-lg font-semibold text-gray-900">팀원 관리</h2>
          <div className="flex gap-2">
            <Link
              href="/dashboard/settings/billing"
              className="rounded-lg border border-gray-300 px-4 py-2 text-sm font-medium text-gray-700 hover:bg-gray-50"
            >
              구독 & 결제
            </Link>
            <Link
              href="/dashboard/members"
              className="rounded-lg bg-blue-600 px-4 py-2 text-sm font-medium text-white hover:bg-blue-700"
            >
              팀원 초대
            </Link>
          </div>
        </div>
      </div>
    </main>
  );
}
