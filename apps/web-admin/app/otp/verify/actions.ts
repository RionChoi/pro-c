"use server";

import { auth, unstable_update } from "@/auth";
import { prisma } from "@repo/db";
import speakeasy from "speakeasy";
import { redirect } from "next/navigation";
import { decryptSecret } from "@/lib/otp-crypto";

const MAX_ATTEMPTS = 5;
const LOCK_DURATION_MS = 15 * 60 * 1000; // 15분

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

  if (user.otpLockedUntil && user.otpLockedUntil > new Date()) {
    redirect("/otp/verify?error=locked");
  }

  const isValid = speakeasy.totp.verify({
    secret: decryptSecret(user.totpSecret),
    encoding: "base32",
    token: code,
    window: 1,
  });

  if (!isValid) {
    const newAttempts = user.otpFailedAttempts + 1;
    await prisma.user.update({
      where: { email: session.user.email },
      data: {
        otpFailedAttempts: newAttempts,
        otpLockedUntil:
          newAttempts >= MAX_ATTEMPTS
            ? new Date(Date.now() + LOCK_DURATION_MS)
            : null,
      },
    });
    redirect(
      newAttempts >= MAX_ATTEMPTS
        ? "/otp/verify?error=locked"
        : "/otp/verify?error=invalid"
    );
  }

  await prisma.user.update({
    where: { email: session.user.email },
    data: { otpFailedAttempts: 0, otpLockedUntil: null },
  });

  await unstable_update({ otpVerified: true });
  redirect("/dashboard");
}
