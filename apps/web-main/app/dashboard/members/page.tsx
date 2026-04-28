import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import { inviteMemberAction } from "./actions";
import Link from "next/link";

const ROLE_LABELS: Record<string, string> = {
  ADMIN: "관리자",
  MEMBER: "멤버",
  PARTNER: "파트너",
  VIEWER: "뷰어",
};

interface Props {
  searchParams: Promise<{ success?: string; error?: string }>;
}

export default async function MembersPage({ searchParams }: Props) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { id: true, tenantId: true, role: true },
  });

  if (!user?.tenantId) redirect("/onboarding");

  const [members, pendingInvites] = await Promise.all([
    prisma.tenantMember.findMany({
      where: { tenantId: user.tenantId },
      include: { user: { select: { email: true, name: true, image: true } } },
      orderBy: { joinedAt: "asc" },
    }),
    prisma.invite.findMany({
      where: { tenantId: user.tenantId, usedAt: null, expiresAt: { gt: new Date() } },
      orderBy: { createdAt: "desc" },
    }),
  ]);

  const { success, error } = await searchParams;
  const isAdmin = user.role === "ADMIN";

  return (
    <main className="min-h-screen bg-gray-50">
      <nav className="border-b bg-white px-6 py-4 flex items-center gap-4">
        <Link href="/dashboard" className="text-sm text-gray-500 hover:text-gray-900">
          ← 대시보드
        </Link>
        <span className="text-gray-300">|</span>
        <span className="font-semibold text-gray-900">팀원 관리</span>
      </nav>

      <div className="mx-auto max-w-3xl px-6 py-10 space-y-8">
        {success && (
          <div className="rounded-md border border-green-200 bg-green-50 px-4 py-3 text-sm text-green-700">
            초대 이메일이 발송되었습니다.
          </div>
        )}
        {error && (
          <div className="rounded-md border border-red-200 bg-red-50 px-4 py-3 text-sm text-red-700">
            {error === "already-member" ? "이미 팀원입니다." : "오류가 발생했습니다."}
          </div>
        )}

        {/* 멤버 목록 */}
        <section>
          <h2 className="mb-3 text-base font-semibold text-gray-900">
            현재 멤버 ({members.length}명)
          </h2>
          <div className="overflow-hidden rounded-xl border bg-white shadow-sm">
            {members.map(({ id, user: member, role, joinedAt }) => (
              <div
                key={id}
                className="flex items-center justify-between px-5 py-4 border-b last:border-0"
              >
                <div className="flex items-center gap-3">
                  <div className="flex h-9 w-9 items-center justify-center rounded-full bg-blue-100 text-sm font-semibold text-blue-700">
                    {(member.name ?? member.email).charAt(0).toUpperCase()}
                  </div>
                  <div>
                    <p className="text-sm font-medium text-gray-900">{member.name ?? member.email}</p>
                    <p className="text-xs text-gray-400">{member.email}</p>
                  </div>
                </div>
                <div className="flex items-center gap-3">
                  <span className="text-xs text-gray-400">
                    {joinedAt.toLocaleDateString("ko-KR")} 합류
                  </span>
                  <span className="rounded-full bg-gray-100 px-2.5 py-0.5 text-xs font-medium text-gray-600">
                    {ROLE_LABELS[role] ?? role}
                  </span>
                </div>
              </div>
            ))}
          </div>
        </section>

        {/* 초대 대기 목록 */}
        {pendingInvites.length > 0 && (
          <section>
            <h2 className="mb-3 text-base font-semibold text-gray-900">
              초대 대기 중 ({pendingInvites.length})
            </h2>
            <div className="overflow-hidden rounded-xl border bg-white shadow-sm">
              {pendingInvites.map((invite) => (
                <div
                  key={invite.id}
                  className="flex items-center justify-between px-5 py-4 border-b last:border-0"
                >
                  <div>
                    <p className="text-sm font-medium text-gray-900">{invite.email}</p>
                    <p className="text-xs text-gray-400">
                      만료: {invite.expiresAt.toLocaleDateString("ko-KR")}
                    </p>
                  </div>
                  <span className="rounded-full bg-yellow-50 px-2.5 py-0.5 text-xs font-medium text-yellow-700">
                    {ROLE_LABELS[invite.role] ?? invite.role} · 대기중
                  </span>
                </div>
              ))}
            </div>
          </section>
        )}

        {/* 초대 폼 */}
        {isAdmin && (
          <section>
            <h2 className="mb-3 text-base font-semibold text-gray-900">팀원 초대</h2>
            <div className="rounded-xl border bg-white p-6 shadow-sm">
              <form action={inviteMemberAction} className="space-y-4">
                <div className="flex gap-3">
                  <input
                    name="email"
                    type="email"
                    required
                    placeholder="이메일 주소"
                    className="flex-1 h-10 rounded-md border border-gray-300 px-3 py-2 text-sm placeholder:text-gray-400 focus:outline-none focus:ring-2 focus:ring-blue-500"
                  />
                  <select
                    name="role"
                    defaultValue="MEMBER"
                    className="h-10 rounded-md border border-gray-300 px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500"
                  >
                    <option value="MEMBER">멤버</option>
                    <option value="VIEWER">뷰어</option>
                    <option value="ADMIN">관리자</option>
                  </select>
                  <button
                    type="submit"
                    className="h-10 rounded-lg bg-blue-600 px-5 text-sm font-medium text-white hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500"
                  >
                    초대
                  </button>
                </div>
                <p className="text-xs text-gray-400">
                  초대 링크가 생성되어 이메일로 발송됩니다. 링크는 7일간 유효합니다.
                </p>
              </form>
            </div>
          </section>
        )}
      </div>
    </main>
  );
}
