import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";

export default async function DashboardPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    include: {
      tenant: true,
    },
  });

  if (!user?.tenant) redirect("/onboarding");

  const { tenant } = user;

  return (
    <main className="min-h-screen bg-gray-50">
      <nav className="border-b bg-white px-6 py-4 flex items-center justify-between">
        <span className="font-semibold text-gray-900">{tenant.name}</span>
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

        <div className="grid grid-cols-1 gap-4 sm:grid-cols-3">
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">플랜</p>
            <p className="mt-1 text-2xl font-bold text-gray-900">{tenant.plan}</p>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">슬러그</p>
            <p className="mt-1 text-2xl font-bold text-gray-900">{tenant.slug}</p>
          </div>
          <div className="rounded-xl border bg-white p-6 shadow-sm">
            <p className="text-sm text-gray-500">생성일</p>
            <p className="mt-1 text-2xl font-bold text-gray-900">
              {tenant.createdAt.toLocaleDateString("ko-KR")}
            </p>
          </div>
        </div>
      </div>
    </main>
  );
}
