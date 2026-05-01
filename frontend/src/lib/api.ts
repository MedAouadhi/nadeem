import { getAccessToken } from "./jwt";

const BASE = process.env.BACKEND_INTERNAL_URL!;

export async function backend<T>(path: string, init: RequestInit = {}): Promise<T> {
  const token = await getAccessToken();
  const headers = new Headers(init.headers as HeadersInit);
  if (token) headers.set("Authorization", `Bearer ${token}`);
  if (!headers.has("Content-Type") && init.body) headers.set("Content-Type", "application/json");
  const res = await fetch(`${BASE}${path}`, { ...init, headers, cache: "no-store" });
  if (!res.ok) throw new BackendError(res.status, await res.text());
  return res.status === 204 ? (undefined as T) : (res.json() as Promise<T>);
}

export class BackendError extends Error {
  constructor(public status: number, body: string) { super(`backend ${status}: ${body}`); }
}
