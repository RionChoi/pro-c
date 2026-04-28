import { auth } from "@/auth";
import { prisma } from "@repo/db";
import { redirect } from "next/navigation";
import { verifyOtpAction } from "./actions";

const ERROR_MESSAGES: Record<string, string> = {
  invalid: "인증 코드가 올바르지 않습니다. 다시 시도해주세요.",
  required: "인증 코드를 입력해주세요.",
};

interface Props {
  searchParams: Promise<{ error?: string }>;
}

export default async function OtpVerifyPage({ searchParams }: Props) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { totpEnabled: true },
  });

  if (!user?.totpEnabled) redirect("/otp/setup");

  const { error } = await searchParams;

  return (
    <main className="flex min-h-screen items-center justify-center bg-gray-50">
      <div className="w-full max-w-sm rounded-xl border bg-white p-8 shadow-sm">
        <div className="mb-6 text-center">
          <div className="mx-auto mb-3 flex h-12 w-12 items-center justify-center rounded-full bg-blue-50">
            <svg className="h-6 w-6 text-blue-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
            </svg>
          </div>
          <h1 className="text-xl font-bold text-gray-900">2단계 인증</h1>
          <p className="mt-1 text-sm text-gray-500">
            인증 앱의 6자리 코드를 입력하세요
          </p>
        </div>

        {error && (
          <div className="mb-4 rounded-md border border-red-200 bg-red-50 px-4 py-3 text-sm text-red-700">
            {ERROR_MESSAGES[error] ?? "오류가 발생했습니다."}
          </div>
        )}

        <form action={verifyOtpAction} className="space-y-4">
          <input
            name="code"
            type="text"
            inputMode="numeric"
            pattern="[0-9]{6}"
            maxLength={6}
            required
            placeholder="000000"
            autoFocus
            autoComplete="one-time-code"
            className="flex h-12 w-full rounded-md border border-gray-300 bg-white px-3 text-center text-2xl tracking-widest placeholder:text-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500"
          />
          <button
            type="submit"
            className="w-full rounded-lg bg-blue-600 px-4 py-2.5 text-sm font-medium text-white hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500"
          >
            인증하기
          </button>
        </form>
      </div>
    </main>
  );
}
