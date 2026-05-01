import { NextResponse } from "next/server";
import { backend } from "@/lib/api";

export async function GET() {
  try {
    const data = await backend("/api/auth/me");
    return NextResponse.json(data);
  } catch {
    return NextResponse.json({ detail: "unauthorized" }, { status: 401 });
  }
}
