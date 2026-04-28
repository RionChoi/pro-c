import { NextResponse } from "next/server";
import { auth } from "@/auth";
import { hasRole } from "@repo/auth";
import type { Role } from "@repo/auth";

type Ok = { ok: true; role: Role };
type Fail = { ok: false; response: NextResponse };

export async function requireRole(...roles: Role[]): Promise<Ok | Fail> {
  const session = await auth();
  if (!session?.user) {
    return { ok: false, response: NextResponse.json({ error: "Unauthorized" }, { status: 401 }) };
  }
  if (!hasRole(session.user.role, ...roles)) {
    return { ok: false, response: NextResponse.json({ error: "Forbidden" }, { status: 403 }) };
  }
  return { ok: true, role: session.user.role as Role };
}
