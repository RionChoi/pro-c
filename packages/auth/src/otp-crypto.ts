import { createCipheriv, createDecipheriv, randomBytes } from "crypto";

const ALGORITHM = "aes-256-gcm";
const KEY_HEX_LENGTH = 64;

function getKey(): Buffer {
  const raw = process.env.TOTP_ENCRYPTION_KEY ?? "";
  if (raw.length !== KEY_HEX_LENGTH) {
    throw new Error("TOTP_ENCRYPTION_KEY must be a 64-char hex string (32 bytes)");
  }
  return Buffer.from(raw, "hex");
}

export function encryptSecret(plaintext: string): string {
  const key = getKey();
  const iv = randomBytes(12);
  const cipher = createCipheriv(ALGORITHM, key, iv);
  const encrypted = Buffer.concat([cipher.update(plaintext, "utf8"), cipher.final()]);
  const authTag = cipher.getAuthTag();
  return `${iv.toString("hex")}:${authTag.toString("hex")}:${encrypted.toString("hex")}`;
}

export function decryptSecret(stored: string): string {
  const parts = stored.split(":");
  if (parts.length !== 3) throw new Error("Invalid encrypted secret format");
  const [ivHex, authTagHex, encryptedHex] = parts as [string, string, string];
  const key = getKey();
  const decipher = createDecipheriv(ALGORITHM, key, Buffer.from(ivHex, "hex"));
  decipher.setAuthTag(Buffer.from(authTagHex, "hex"));
  return decipher.update(Buffer.from(encryptedHex, "hex")) + decipher.final("utf8");
}
