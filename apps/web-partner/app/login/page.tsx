import { signIn } from "@/auth";

interface Props {
  searchParams: Promise<{ error?: string }>;
}

export default async function PartnerLoginPage({ searchParams }: Props) {
  const { error } = await searchParams;

  return (
    <main className="flex min-h-screen items-center justify-center">
      <div className="w-full max-w-sm space-y-4 p-8">
        <h1 className="text-2xl font-bold text-center">파트너 로그인</h1>
        <p className="text-sm text-center text-gray-500">
          PARTNER 권한 계정만 접근 가능합니다
        </p>
        {error && (
          <p className="text-sm text-center text-red-500">
            접근 권한이 없습니다. PARTNER 계정으로 로그인해주세요.
          </p>
        )}
        <form
          action={async () => {
            "use server";
            await signIn("google", { redirectTo: "/" });
          }}
        >
          <button
            type="submit"
            className="w-full rounded-lg bg-green-600 px-4 py-2 text-white hover:bg-green-700"
          >
            Google로 로그인
          </button>
        </form>
      </div>
    </main>
  );
}
