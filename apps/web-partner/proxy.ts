import { getToken } from "next-auth/jwt";
import { hasRole } from "@repo/auth/rbac";
import { NextResponse, type NextRequest } from "next/server";

export async function proxy(req: NextRequest) {
  const token = await getToken({ req, secret: process.env.AUTH_SECRET });
  const { pathname } = req.nextUrl;

  const isLoggedIn = !!token;
  const isAuthPage = pathname.startsWith("/login");
  const isOtpPage = pathname.startsWith("/otp");

  if (!isLoggedIn && !isAuthPage) {
    return NextResponse.redirect(new URL("/login", req.url));
  }
  if (isLoggedIn && isAuthPage) {
    return NextResponse.redirect(new URL("/", req.url));
  }

  if (isLoggedIn) {
    const role = token?.role as string | undefined;
    if (!hasRole(role, "PARTNER")) {
      return NextResponse.redirect(new URL("/login?error=forbidden", req.url));
    }

    if (!isOtpPage && !token?.otpVerified) {
      return NextResponse.redirect(new URL("/otp/verify", req.url));
    }
  }

  return NextResponse.next();
}

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
