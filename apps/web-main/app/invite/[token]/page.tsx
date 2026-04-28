import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import { acceptInviteAction } from "./actions";

interface Props {
  params: Promise<{ token: string }>;
}

export default async function InvitePage({ params }: Props) {
  const { token } = await params;

  const invite = await prisma.invite.findUnique({
    where: { token },
    include: { tenant: { select: { name: true, slug: true } } },
  });

  if (!invite || invite.usedAt || invite.expiresAt < new Date()) {
    return (
      <main className="flex min-h-screen items-center justify-center bg-gray-50">
        <div className="w-full max-w-md rounded-xl border bg-white p-8 text-center shadow-sm">
          <p className="text-lg font-semibold text-gray-900">유효하지 않은 초대 링크입니다.</p>
          <p className="mt-2 text-sm text-gray-500">링크가 만료되었거나 이미 사용되었습니다.</p>
          <a href="/login" className="mt-6 inline-block text-sm text-blue-600 hover:underline">
            로그인으로 이동
          </a>
        </div>
      </main>
    );
  }

  const session = await auth();

  return (
    <main className="flex min-h-screen items-center justify-center bg-gray-50">
      <div className="w-full max-w-md rounded-xl border bg-white p-8 shadow-sm">
        <div className="mb-6 text-center">
          <h1 className="text-2xl font-bold text-gray-900">초대를 받으셨습니다</h1>
          <p className="mt-2 text-sm text-gray-500">
            <span className="font-medium text-gray-900">{invite.tenant.name}</span> 워크스페이스에 참여하세요.
          </p>
        </div>

        <div className="mb-6 rounded-lg bg-gray-50 p-4 text-sm">
          <div className="flex justify-between text-gray-600">
            <span>워크스페이스</span>
            <span className="font-medium text-gray-900">{invite.tenant.name}</span>
          </div>
          <div className="mt-2 flex justify-between text-gray-600">
            <span>초대된 이메일</span>
            <span className="font-medium text-gray-900">{invite.email}</span>
          </div>
          <div className="mt-2 flex justify-between text-gray-600">
            <span>역할</span>
            <span className="font-medium text-gray-900">{invite.role}</span>
          </div>
        </div>

        {session ? (
          <form action={acceptInviteAction}>
            <input type="hidden" name="token" value={token} />
            <button
              type="submit"
              className="w-full rounded-lg bg-blue-600 px-4 py-2.5 text-sm font-medium text-white hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500"
            >
              {invite.tenant.name} 참여하기
            </button>
          </form>
        ) : (
          <a
            href={`/login?callbackUrl=/invite/${token}`}
            className="block w-full rounded-lg bg-blue-600 px-4 py-2.5 text-center text-sm font-medium text-white hover:bg-blue-700"
          >
            로그인 후 참여하기
          </a>
        )}
      </div>
    </main>
  );
}
