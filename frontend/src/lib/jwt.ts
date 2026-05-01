import { cookies } from "next/headers";

const COOKIE = "nadeem_jwt";

export async function setJwtCookie(access: string, refresh: string) {
  const c = await cookies();
  c.set(COOKIE, access, { httpOnly: true, sameSite: "lax", secure: process.env.NODE_ENV === "production", path: "/" });
  c.set("nadeem_refresh", refresh, { httpOnly: true, sameSite: "lax", secure: process.env.NODE_ENV === "production", path: "/" });
}

export async function clearJwtCookie() {
  const c = await cookies();
  c.delete(COOKIE);
  c.delete("nadeem_refresh");
}

export async function getAccessToken(): Promise<string | null> {
  const c = await cookies();
  return c.get(COOKIE)?.value ?? null;
}
