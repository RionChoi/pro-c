import { auth } from "@/auth";
import { prisma } from "@repo/db";
import { redirect } from "next/navigation";
import speakeasy from "speakeasy";
import QRCode from "qrcode";
import { cookies } from "next/headers";
import { confirmOtpSetupAction } from "./actions";

const ERROR_MESSAGES: Record<string, string> = {
  invalid: "인증 코드가 올바르지 않습니다. 다시 시도해주세요.",
  required: "인증 코드를 입력해주세요.",
  expired: "설정 세션이 만료됐습니다. 페이지를 새로고침 해주세요.",
};

interface Props {
  searchParams: Promise<{ error?: string }>;
}

export default async function OtpSetupPage({ searchParams }: Props) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { totpEnabled: true },
  });

  if (user?.totpEnabled) redirect("/otp/verify");

  const cookieStore = await cookies();
  let secret = cookieStore.get("totp_setup_secret")?.value;

  if (!secret) {
    const generated = speakeasy.generateSecret({ name: `Platform Admin (${session.user.email})` });
    secret = generated.base32;
    cookieStore.set("totp_setup_secret", secret, {
      httpOnly: true,
      secure: process.env.NODE_ENV === "production",
      maxAge: 600,
      path: "/",
    });
  }

  const otpauth = speakeasy.otpauthURL({
    secret,
    label: session.user.email,
    issuer: "Platform Admin",
    encoding: "base32",
  });
  const qrDataUrl = await QRCode.toDataURL(otpauth, { width: 200, margin: 2 });

  const { error } = await searchParams;

  return (
    <main className="flex min-h-screen items-center justify-center bg-gray-50">
      <div className="w-full max-w-sm rounded-xl border bg-white p-8 shadow-sm">
        <div className="mb-6 text-center">
          <h1 className="text-xl font-bold text-gray-900">2단계 인증 설정</h1>
          <p className="mt-1 text-sm text-gray-500">
            Google Authenticator 또는 호환 앱으로 QR코드를 스캔하세요
          </p>
        </div>

        <div className="mb-6 flex justify-center">
          {/* eslint-disable-next-line @next/next/no-img-element */}
          <img src={qrDataUrl} alt="OTP QR Code" width={200} height={200} className="rounded-md border" />
        </div>

        <details className="mb-6">
          <summary className="cursor-pointer text-xs text-gray-400 hover:text-gray-600">
            QR코드를 스캔할 수 없나요?
          </summary>
          <p className="mt-2 break-all rounded bg-gray-50 p-2 font-mono text-xs text-gray-700">
            {secret}
          </p>
        </details>

        {error && (
          <div className="mb-4 rounded-md border border-red-200 bg-red-50 px-4 py-3 text-sm text-red-700">
            {ERROR_MESSAGES[error] ?? "오류가 발생했습니다."}
          </div>
        )}

        <form action={confirmOtpSetupAction} className="space-y-4">
          <div>
            <label className="mb-1.5 block text-sm font-medium text-gray-700">
              앱에 표시된 6자리 코드 입력
            </label>
            <input
              name="code"
              type="text"
              inputMode="numeric"
              pattern="[0-9]{6}"
              maxLength={6}
              required
              placeholder="000000"
              autoComplete="one-time-code"
              className="flex h-12 w-full rounded-md border border-gray-300 bg-white px-3 text-center text-2xl tracking-widest placeholder:text-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500"
            />
          </div>
          <button
            type="submit"
            className="w-full rounded-lg bg-blue-600 px-4 py-2.5 text-sm font-medium text-white hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500"
          >
            설정 완료
          </button>
        </form>
      </div>
    </main>
  );
}
