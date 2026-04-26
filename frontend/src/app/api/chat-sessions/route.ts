import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/chat-sessions")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
