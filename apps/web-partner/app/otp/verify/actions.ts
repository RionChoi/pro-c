"use server";

import { auth, unstable_update } from "@/auth";
import { prisma } from "@repo/db";
import { redirect } from "next/navigation";
import { verifyTotpToken, isOtpLocked, nextLockoutState, MAX_OTP_ATTEMPTS } from "@repo/auth";

export async function verifyOtpAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const code = (formData.get("code") as string)?.trim();
  if (!code) redirect("/otp/verify?error=required");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: {
      totpSecret: true,
      totpEnabled: true,
      otpFailedAttempts: true,
      otpLockedUntil: true,
    },
  });

  if (!user?.totpEnabled || !user.totpSecret) redirect("/otp/setup");

  if (isOtpLocked(user.otpLockedUntil)) redirect("/otp/verify?error=locked");

  const isValid = verifyTotpToken(user.totpSecret, code);

  if (!isValid) {
    const lockout = nextLockoutState(user.otpFailedAttempts);
    await prisma.user.update({
      where: { email: session.user.email },
      data: lockout,
    });
    redirect(
      lockout.otpFailedAttempts >= MAX_OTP_ATTEMPTS
        ? "/otp/verify?error=locked"
        : "/otp/verify?error=invalid"
    );
  }

  await prisma.user.update({
    where: { email: session.user.email },
    data: { otpFailedAttempts: 0, otpLockedUntil: null },
  });

  await unstable_update({ otpVerified: true });
  redirect("/");
}
