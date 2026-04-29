import { auth } from "@/auth";
import { hasRole } from "@repo/auth";
import { NextResponse } from "next/server";

export default auth((req) => {
  const { pathname } = req.nextUrl;
  const isLoggedIn = !!req.auth;
  const isAuthPage = pathname.startsWith("/login");
  const isOtpPage = pathname.startsWith("/otp");

  if (!isLoggedIn && !isAuthPage) {
    return NextResponse.redirect(new URL("/login", req.url));
  }
  if (isLoggedIn && isAuthPage) {
    return NextResponse.redirect(new URL("/", req.url));
  }

  if (isLoggedIn) {
    const role = req.auth?.user?.role;
    if (!hasRole(role, "PARTNER")) {
      return NextResponse.redirect(new URL("/login?error=forbidden", req.url));
    }

    if (!isOtpPage && !req.auth?.otpVerified) {
      return NextResponse.redirect(new URL("/otp/verify", req.url));
    }
  }

  return NextResponse.next();
});

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
