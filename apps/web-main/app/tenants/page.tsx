import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import { selectTenantAction } from "./actions";

export default async function TenantSelectPage() {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    include: {
      memberships: {
        include: { tenant: true },
        orderBy: { joinedAt: "desc" },
      },
    },
  });

  if (!user) redirect("/login");
  if (user.memberships.length === 0) redirect("/onboarding");
  if (user.memberships.length === 1) {
    const only = user.memberships[0];
    if (only) {
      await prisma.user.update({
        where: { id: user.id },
        data: { tenantId: only.tenantId },
      });
    }
    redirect("/dashboard");
  }

  return (
    <main className="flex min-h-screen items-center justify-center bg-gray-50">
      <div className="w-full max-w-lg px-4">
        <div className="mb-8 text-center">
          <h1 className="text-2xl font-bold text-gray-900">워크스페이스 선택</h1>
          <p className="mt-2 text-sm text-gray-500">
            계속할 워크스페이스를 선택하세요.
          </p>
        </div>

        <div className="space-y-3">
          {user.memberships.map(({ tenant, role }) => (
            <form key={tenant.id} action={selectTenantAction}>
              <input type="hidden" name="tenantId" value={tenant.id} />
              <button
                type="submit"
                className="w-full rounded-xl border bg-white p-5 text-left shadow-sm transition hover:border-blue-400 hover:shadow-md focus:outline-none focus:ring-2 focus:ring-blue-500"
              >
                <div className="flex items-center justify-between">
                  <div>
                    <p className="font-semibold text-gray-900">{tenant.name}</p>
                    <p className="mt-0.5 text-sm text-gray-400">{tenant.slug}</p>
                  </div>
                  <div className="flex flex-col items-end gap-1">
                    <span className="rounded-full bg-blue-50 px-2.5 py-0.5 text-xs font-medium text-blue-700">
                      {tenant.plan}
                    </span>
                    <span className="text-xs text-gray-400">{role}</span>
                  </div>
                </div>
              </button>
            </form>
          ))}
        </div>

        <p className="mt-6 text-center text-sm text-gray-400">
          새 워크스페이스를 만들려면{" "}
          <a href="/onboarding" className="text-blue-600 hover:underline">
            여기
          </a>
          를 클릭하세요.
        </p>
      </div>
    </main>
  );
}
