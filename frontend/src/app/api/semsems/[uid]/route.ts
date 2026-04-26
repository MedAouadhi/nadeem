import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET(_req: Request, { params }: { params: Promise<{ uid: string }> }) {
  const { uid } = await params;
  try { return NextResponse.json(await backend(`/api/semsems/${uid}`)); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
