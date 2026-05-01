import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function POST(req: Request) {
  try {
    const body = await req.json();
    return NextResponse.json(await backend("/api/devices/provision", { method: "POST", body: JSON.stringify(body) }));
  } catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
