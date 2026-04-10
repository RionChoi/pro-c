import { signIn } from "@/auth";

export default function AdminLoginPage({
  searchParams,
}: {
  searchParams: { error?: string };
}) {
  return (
    <main className="flex min-h-screen items-center justify-center bg-gray-950">
      <div className="w-full max-w-sm space-y-4 p-8 rounded-xl bg-gray-900 border border-gray-800">
        <div className="text-center space-y-2">
          <h1 className="text-2xl font-bold text-white">관리자 로그인</h1>
          <p className="text-sm text-gray-400">관리자 권한이 필요합니다</p>
        </div>

        {searchParams?.error === "unauthorized" && (
          <div className="rounded-lg bg-red-900/30 border border-red-800 p-3 text-sm text-red-300 text-center">
            ⛔ 관리자 권한이 없는 계정입니다.
          </div>
        )}

        <form
          action={async () => {
            "use server";
            await signIn("google", { redirectTo: "/" });
          }}
        >
          <button
            type="submit"
            className="w-full rounded-lg bg-indigo-600 px-4 py-2.5 text-white font-medium hover:bg-indigo-700 transition-colors"
          >
            Google로 로그인
          </button>
        </form>
      </div>
    </main>
  );
}
