"use server";

import { auth, unstable_update } from "@/auth";
import { prisma } from "@repo/db";
import speakeasy from "speakeasy";
import { cookies } from "next/headers";
import { redirect } from "next/navigation";

export async function confirmOtpSetupAction(formData: FormData) {
  const session = await auth();
  if (!session?.user?.email) redirect("/login");

  const code = (formData.get("code") as string)?.trim();
  if (!code) redirect("/otp/setup?error=required");

  const cookieStore = await cookies();
  const secret = cookieStore.get("totp_setup_secret")?.value;
  if (!secret) redirect("/otp/setup?error=expired");

  const isValid = speakeasy.totp.verify({
    secret,
    encoding: "base32",
    token: code,
    window: 1,
  });
  if (!isValid) redirect("/otp/setup?error=invalid");

  await prisma.user.update({
    where: { email: session.user.email },
    data: { totpSecret: secret, totpEnabled: true },
  });

  cookieStore.delete("totp_setup_secret");
  await unstable_update({ otpVerified: true });
  redirect("/dashboard");
}
