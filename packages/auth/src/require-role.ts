import { NextResponse } from "next/server";
import type { Session } from "next-auth";
import { hasRole } from "./rbac";
import type { Role } from "./rbac";

type Ok = { ok: true; role: Role };
type Fail = { ok: false; response: NextResponse };

export function requireRole(session: Session | null, ...roles: Role[]): Ok | Fail {
  if (!session?.user) {
    return { ok: false, response: NextResponse.json({ error: "Unauthorized" }, { status: 401 }) };
  }
  if (!hasRole(session.user.role, ...roles)) {
    return { ok: false, response: NextResponse.json({ error: "Forbidden" }, { status: 403 }) };
  }
  return { ok: true, role: session.user.role as Role };
}
