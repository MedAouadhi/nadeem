import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET(_req: Request, { params }: { params: Promise<{ id: string }> }) {
  const { id } = await params;
  try { return NextResponse.json(await backend(`/api/chat-sessions/${id}`)); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
