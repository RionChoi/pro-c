-- AlterTable
ALTER TABLE "User" ADD COLUMN "otpFailedAttempts" INTEGER NOT NULL DEFAULT 0,
ADD COLUMN "otpLockedUntil" TIMESTAMP(3);
