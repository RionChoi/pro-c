import speakeasy from "speakeasy";
import { decryptSecret } from "./otp-crypto";

export const MAX_OTP_ATTEMPTS = 5;
export const OTP_LOCK_DURATION_MS = 15 * 60 * 1000;

export function generateTotpSecret(label: string, issuer: string): { base32: string; otpauthUrl: string } {
  const generated = speakeasy.generateSecret({ name: label });
  const base32 = generated.base32!;
  const otpauthUrl = speakeasy.otpauthURL({ secret: base32, label, issuer, encoding: "base32" });
  return { base32, otpauthUrl };
}

export function totpOtpauthUrl(base32: string, label: string, issuer: string): string {
  return speakeasy.otpauthURL({ secret: base32, label, issuer, encoding: "base32" });
}

export function verifyRawTotpToken(rawSecret: string, code: string): boolean {
  return speakeasy.totp.verify({ secret: rawSecret, encoding: "base32", token: code, window: 1 });
}

export function verifyTotpToken(encryptedSecret: string, code: string): boolean {
  return speakeasy.totp.verify({
    secret: decryptSecret(encryptedSecret),
    encoding: "base32",
    token: code,
    window: 1,
  });
}

export function isOtpLocked(otpLockedUntil: Date | null): boolean {
  return otpLockedUntil != null && otpLockedUntil > new Date();
}

export function nextLockoutState(currentAttempts: number): {
  otpFailedAttempts: number;
  otpLockedUntil: Date | null;
} {
  const next = currentAttempts + 1;
  return {
    otpFailedAttempts: next,
    otpLockedUntil: next >= MAX_OTP_ATTEMPTS ? new Date(Date.now() + OTP_LOCK_DURATION_MS) : null,
  };
}
