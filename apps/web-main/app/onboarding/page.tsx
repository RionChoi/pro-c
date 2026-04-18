import { auth } from "@/auth";
import { redirect } from "next/navigation";
import { prisma } from "@repo/db";
import { createTenantAction } from "./actions";

const ERROR_MESSAGES: Record<string, string> = {
  required: "이름과 슬러그는 필수입니다.",
  "invalid-slug": "슬러그는 소문자, 숫자, 하이픈만 사용 가능합니다.",
  "slug-taken": "이미 사용 중인 슬러그입니다. 다른 슬러그를 입력해주세요.",
};

interface Props {
  searchParams: Promise<{ error?: string }>;
}

export default async function OnboardingPage({ searchParams }: Props) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { tenantId: true },
  });

  if (user?.tenantId) redirect("/dashboard");

  const { error } = await searchParams;

  return (
    <main className="flex min-h-screen items-center justify-center bg-gray-50">
      <div className="w-full max-w-md rounded-xl border bg-white p-8 shadow-sm">
        <div className="mb-6">
          <h1 className="text-2xl font-bold text-gray-900">워크스페이스 만들기</h1>
          <p className="mt-1 text-sm text-gray-500">
            팀을 위한 전용 공간을 생성합니다.
          </p>
        </div>

        {error && (
          <div className="mb-4 rounded-md border border-red-200 bg-red-50 px-4 py-3 text-sm text-red-700">
            {ERROR_MESSAGES[error] ?? "오류가 발생했습니다. 다시 시도해주세요."}
          </div>
        )}

        <form action={createTenantAction} className="space-y-5">
          <div className="space-y-1.5">
            <label htmlFor="name" className="text-sm font-medium text-gray-700">
              워크스페이스 이름
            </label>
            <input
              id="name"
              name="name"
              type="text"
              required
              placeholder="예: Acme Corp"
              className="flex h-10 w-full rounded-md border border-gray-300 bg-white px-3 py-2 text-sm placeholder:text-gray-400 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-transparent"
            />
          </div>

          <div className="space-y-1.5">
            <label htmlFor="slug" className="text-sm font-medium text-gray-700">
              슬러그 (URL 식별자)
            </label>
            <input
              id="slug"
              name="slug"
              type="text"
              required
              placeholder="예: acme-corp"
              pattern="^[a-z0-9-]+$"
              title="소문자, 숫자, 하이픈만 사용 가능합니다"
              className="flex h-10 w-full rounded-md border border-gray-300 bg-white px-3 py-2 text-sm placeholder:text-gray-400 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-transparent"
            />
            <p className="text-xs text-gray-400">소문자, 숫자, 하이픈(-) 만 사용 가능</p>
          </div>

          <div className="space-y-1.5">
            <label htmlFor="plan" className="text-sm font-medium text-gray-700">
              플랜
            </label>
            <select
              id="plan"
              name="plan"
              defaultValue="FREE"
              className="flex h-10 w-full rounded-md border border-gray-300 bg-white px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-transparent"
            >
              <option value="FREE">Free</option>
              <option value="STARTER">Starter</option>
              <option value="PRO">Pro</option>
              <option value="ENTERPRISE">Enterprise</option>
            </select>
          </div>

          <button
            type="submit"
            className="w-full rounded-lg bg-blue-600 px-4 py-2.5 text-sm font-medium text-white hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2"
          >
            워크스페이스 생성
          </button>
        </form>
      </div>
    </main>
  );
}
