import { NextRequest, NextResponse } from "next/server";

const PUBLIC = ["/login", "/signup", "/api/auth/login", "/api/auth/signup", "/nadeem-logo.svg"];

export function middleware(req: NextRequest) {
  const url = req.nextUrl;
  if (PUBLIC.some((p) => url.pathname === p || url.pathname.startsWith(p + "/"))) return NextResponse.next();
  const token = req.cookies.get("nadeem_jwt")?.value;
  if (!token && !url.pathname.startsWith("/api")) {
    const r = url.clone();
    r.pathname = "/login";
    return NextResponse.redirect(r);
  }
  if (!token && url.pathname.startsWith("/api")) {
    return NextResponse.json({ detail: "unauthorized" }, { status: 401 });
  }
  return NextResponse.next();
}

export const config = { matcher: ["/((?!_next/static|_next/image|favicon.ico).*))"] };
