import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function DELETE(_req: Request, { params }: { params: Promise<{ id: string }> }) {
  const { id } = await params;
  try { await backend(`/api/devices/${id}`, { method: "DELETE" }); return new Response(null, { status: 204 }); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
