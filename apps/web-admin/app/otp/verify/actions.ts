"use server";

import { auth, unstable_update } from "@/auth";
import { prisma } from "@repo/db";
import speakeasy from "speakeasy";
import { redirect } from "next/navigation";

export async function verifyOtpAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const code = (formData.get("code") as string)?.trim();
  if (!code) redirect("/otp/verify?error=required");

  const user = await prisma.user.findUnique({
    where: { email: session.user.email },
    select: { totpSecret: true, totpEnabled: true },
  });

  if (!user?.totpEnabled || !user.totpSecret) redirect("/otp/setup");

  const isValid = speakeasy.totp.verify({
    secret: user.totpSecret,
    encoding: "base32",
    token: code,
    window: 1,
  });
  if (!isValid) redirect("/otp/verify?error=invalid");

  await unstable_update({ otpVerified: true });
  redirect("/dashboard");
}
