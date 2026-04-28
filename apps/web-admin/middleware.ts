import { auth } from "@/auth";
import { hasRole } from "@repo/auth";
import { NextResponse } from "next/server";

export default auth((req) => {
  const { pathname } = req.nextUrl;
  const isLoggedIn = !!req.auth;
  const isAuthPage = pathname.startsWith("/login");

  if (!isLoggedIn && !isAuthPage) {
    return NextResponse.redirect(new URL("/login", req.url));
  }
  if (isLoggedIn && isAuthPage) {
    return NextResponse.redirect(new URL("/dashboard", req.url));
  }

  // web-admin 전체 → ADMIN 전용
  if (isLoggedIn) {
    const role = req.auth?.user?.role;
    if (!hasRole(role, "ADMIN")) {
      return NextResponse.redirect(new URL("/login?error=forbidden", req.url));
    }
  }

  return NextResponse.next();
});

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
