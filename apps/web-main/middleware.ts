import { auth } from "@/auth";
import { NextResponse } from "next/server";

export default auth((req) => {
  const isLoggedIn = !!req.auth;
  const { pathname } = req.nextUrl;
  const isAuthPage = pathname.startsWith("/login");
  const isPublic = isAuthPage;

  if (!isLoggedIn && !isPublic)
    return NextResponse.redirect(new URL("/login", req.url));
  if (isLoggedIn && isAuthPage)
    return NextResponse.redirect(new URL("/dashboard", req.url));

  return NextResponse.next();
});

export const config = {
  matcher: ["/((?!api|_next/static|_next/image|favicon.ico).*)"],
};
