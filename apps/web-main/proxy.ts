import { getToken } from "next-auth/jwt";
import { hasRole } from "@repo/auth/rbac";
import { NextResponse, type NextRequest } from "next/server";

export async function proxy(req: NextRequest) {
  const token = await getToken({ req, secret: process.env.AUTH_SECRET });
  const { pathname } = req.nextUrl;

  const isLoggedIn = !!token;
  const isAuthPage = pathname.startsWith("/login");
  const isInvitePage = pathname.startsWith("/invite");

  if (!isLoggedIn && !isAuthPage && !isInvitePage) {
    return NextResponse.redirect(new URL("/login", req.url));
  }
  if (isLoggedIn && isAuthPage) {
    return NextResponse.redirect(new URL("/dashboard", req.url));
  }

  if (pathname.startsWith("/admin")) {
    const role = token?.role as string | undefined;
    if (!hasRole(role, "ADMIN")) {
      return NextResponse.redirect(new URL("/dashboard?error=forbidden", req.url));
    }
  }

  return NextResponse.next();
}

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
