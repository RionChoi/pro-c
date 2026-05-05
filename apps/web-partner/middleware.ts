import { auth } from "@/auth";
import { NextResponse } from "next/server";

const PUBLIC_PATHS = ["/login", "/api/auth"];

export default auth((req) => {
  const { pathname } = req.nextUrl;

  // 공개 경로는 통과
  if (PUBLIC_PATHS.some((p) => pathname.startsWith(p))) {
    return NextResponse.next();
  }

  // 미인증 → 로그인
  if (!req.auth) {
    return NextResponse.redirect(new URL("/login", req.url));
  }

  // OTP 경로는 인증만 되면 통과
  if (pathname.startsWith("/otp")) {
    return NextResponse.next();
  }

  // OTP 미검증 → OTP 검증 페이지
  if (!req.auth.otpVerified) {
    return NextResponse.redirect(new URL("/otp/verify", req.url));
  }

  return NextResponse.next();
});

export const config = {
  matcher: ["/((?!_next/static|_next/image|favicon.ico|.*\\.svg$|.*\\.png$).*)"],
};
