import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";

export default async function AdminDashboardPage() {
  const session = await auth();
  if (!session) redirect("/login");

  const tenants = await prisma.tenant.findMany({
    select: {
      id: true,
      name: true,
      slug: true,
      plan: true,
      createdAt: true,
      _count: { select: { users: true } },
    },
    orderBy: { createdAt: "desc" },
  });

  return (
    <main className="min-h-screen bg-gray-50">
      <nav className="border-b bg-white px-6 py-4 flex items-center justify-between">
        <span className="font-semibold text-gray-900">Admin Portal</span>
        <div className="flex items-center gap-4">
          <span className="text-sm text-gray-500">{session.user?.email}</span>
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
        <div className="mb-6 flex items-center justify-between">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">테넌트 관리</h1>
            <p className="mt-1 text-sm text-gray-500">총 {tenants.length}개 테넌트</p>
          </div>
        </div>

        <div className="rounded-xl border bg-white shadow-sm overflow-hidden">
          <table className="w-full text-sm">
            <thead className="border-b bg-gray-50">
              <tr>
                <th className="px-4 py-3 text-left font-medium text-gray-500">이름</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">슬러그</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">플랜</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">유저 수</th>
                <th className="px-4 py-3 text-left font-medium text-gray-500">생성일</th>
              </tr>
            </thead>
            <tbody>
              {tenants.length === 0 ? (
                <tr>
                  <td colSpan={5} className="px-4 py-8 text-center text-gray-400">
                    테넌트가 없습니다.
                  </td>
                </tr>
              ) : (
                tenants.map((tenant) => (
                  <tr key={tenant.id} className="border-b last:border-0 hover:bg-gray-50">
                    <td className="px-4 py-3 font-medium text-gray-900">{tenant.name}</td>
                    <td className="px-4 py-3 text-gray-500 font-mono text-xs">{tenant.slug}</td>
                    <td className="px-4 py-3">
                      <span className="inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-medium bg-blue-50 text-blue-700">
                        {tenant.plan}
                      </span>
                    </td>
                    <td className="px-4 py-3 text-gray-500">{tenant._count.users}</td>
                    <td className="px-4 py-3 text-gray-500">
                      {tenant.createdAt.toLocaleDateString("ko-KR")}
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
