import { describe, it, expect, vi, beforeEach } from "vitest";
import {
  isOtpLocked,
  nextLockoutState,
  MAX_OTP_ATTEMPTS,
  OTP_LOCK_DURATION_MS,
} from "../src/totp";

describe("totp utilities", () => {
  describe("isOtpLocked", () => {
    it("should return false for null lockout", () => {
      expect(isOtpLocked(null)).toBe(false);
    });

    it("should return false for past lockout date", () => {
      const pastDate = new Date(Date.now() - 60000);
      expect(isOtpLocked(pastDate)).toBe(false);
    });

    it("should return true for future lockout date", () => {
      const futureDate = new Date(Date.now() + 60000);
      expect(isOtpLocked(futureDate)).toBe(true);
    });
  });

  describe("nextLockoutState", () => {
    it("should increment failed attempts", () => {
      const result = nextLockoutState(0);
      expect(result.otpFailedAttempts).toBe(1);
    });

    it("should not lock before MAX_OTP_ATTEMPTS", () => {
      for (let i = 0; i < MAX_OTP_ATTEMPTS - 1; i++) {
        const result = nextLockoutState(i);
        expect(result.otpLockedUntil).toBeNull();
      }
    });

    it("should lock at MAX_OTP_ATTEMPTS", () => {
      const result = nextLockoutState(MAX_OTP_ATTEMPTS - 1);
      expect(result.otpFailedAttempts).toBe(MAX_OTP_ATTEMPTS);
      expect(result.otpLockedUntil).not.toBeNull();
    });

    it("should lock beyond MAX_OTP_ATTEMPTS", () => {
      const result = nextLockoutState(MAX_OTP_ATTEMPTS);
      expect(result.otpLockedUntil).not.toBeNull();
    });

    it("lockout duration should be 15 minutes", () => {
      const before = Date.now();
      const result = nextLockoutState(MAX_OTP_ATTEMPTS - 1);
      const after = Date.now();

      expect(result.otpLockedUntil).not.toBeNull();
      const lockTime = result.otpLockedUntil!.getTime();
      expect(lockTime).toBeGreaterThanOrEqual(before + OTP_LOCK_DURATION_MS);
      expect(lockTime).toBeLessThanOrEqual(after + OTP_LOCK_DURATION_MS);
    });
  });

  describe("constants", () => {
    it("MAX_OTP_ATTEMPTS should be 5", () => {
      expect(MAX_OTP_ATTEMPTS).toBe(5);
    });

    it("OTP_LOCK_DURATION_MS should be 15 minutes", () => {
      expect(OTP_LOCK_DURATION_MS).toBe(15 * 60 * 1000);
    });
  });
});
