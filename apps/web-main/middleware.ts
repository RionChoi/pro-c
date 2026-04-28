import { auth } from "@/auth";
import { hasRole } from "@repo/auth";
import { NextResponse } from "next/server";

export default auth((req) => {
  const { pathname } = req.nextUrl;
  const isLoggedIn = !!req.auth;
  const isAuthPage = pathname.startsWith("/login");
  const isInvitePage = pathname.startsWith("/invite");

  if (!isLoggedIn && !isAuthPage && !isInvitePage) {
    return NextResponse.redirect(new URL("/login", req.url));
  }
  if (isLoggedIn && isAuthPage) {
    return NextResponse.redirect(new URL("/dashboard", req.url));
  }

  // /admin/* 경로는 ADMIN 전용
  if (pathname.startsWith("/admin")) {
    const role = req.auth?.user?.role;
    if (!hasRole(role, "ADMIN")) {
      return NextResponse.redirect(new URL("/dashboard?error=forbidden", req.url));
    }
  }

  return NextResponse.next();
});

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
