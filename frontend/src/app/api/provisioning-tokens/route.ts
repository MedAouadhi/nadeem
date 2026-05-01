import { NextRequest, NextResponse } from "next/server";
import { getAccessToken } from "@/lib/jwt";

export async function POST(req: NextRequest) {
  const token = await getAccessToken();
  if (!token) {
    return NextResponse.json({ detail: "unauthorized" }, { status: 401 });
  }

  const r = await fetch(`${process.env.BACKEND_INTERNAL_URL}/api/provisioning-tokens`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "Authorization": `Bearer ${token}`,
    },
  });

  if (!r.ok) {
    const data = await r.json().catch(() => ({}));
    return NextResponse.json({ detail: data.detail || "failed to create token" }, { status: r.status });
  }

  const data = await r.json();
  return NextResponse.json(data);
}
