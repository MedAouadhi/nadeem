import { NextRequest, NextResponse } from "next/server";
import { setJwtCookie } from "@/lib/jwt";

export async function POST(req: NextRequest) {
  const body = await req.json();
  const r = await fetch(`${process.env.BACKEND_INTERNAL_URL}/api/auth/login`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  if (!r.ok) return NextResponse.json({ detail: "invalid credentials" }, { status: 401 });
  const { access, refresh } = await r.json();
  await setJwtCookie(access, refresh);
  return NextResponse.json({ ok: true });
}
