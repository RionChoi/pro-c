import { auth } from "@/auth";
import { NextResponse } from "next/server";

export default auth((req) => {
  const isLoggedIn = !!req.auth;
  const isAuthPage = req.nextUrl.pathname.startsWith("/login");

  if (!isLoggedIn && !isAuthPage)
    return NextResponse.redirect(new URL("/login", req.url));
  if (isLoggedIn && isAuthPage)
    return NextResponse.redirect(new URL("/", req.url));

  return NextResponse.next();
});

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
