import { NextResponse } from "next/server";
import { clearJwtCookie } from "@/lib/jwt";

export async function DELETE() {
  await clearJwtCookie();
  return new NextResponse(null, { status: 204 });
}
