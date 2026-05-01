import { NextRequest, NextResponse } from "next/server";
import { setJwtCookie } from "@/lib/jwt";

export async function POST(req: NextRequest) {
  const body = await req.json();
  const r = await fetch(`${process.env.BACKEND_INTERNAL_URL}/api/auth/signup`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  if (!r.ok) {
    const data = await r.json().catch(() => ({}));
    return NextResponse.json({ detail: data.detail || data.email?.[0] || "signup failed" }, { status: r.status });
  }
  const { access, refresh } = await r.json();
  await setJwtCookie(access, refresh);
  return NextResponse.json({ ok: true });
}
