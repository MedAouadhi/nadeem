import { cookies } from "next/headers";
import { getAccessToken } from "./jwt";

const BASE = process.env.BACKEND_INTERNAL_URL!;
const E2E_MOCK_COOKIE = "nadeem_e2e_backend_mocks";

type E2EMockEntry = unknown | { status: number; body?: unknown };

async function readE2EMock<T>(path: string): Promise<T | null> {
  if (process.env.NODE_ENV === "production") return null;
  const raw = (await cookies()).get(E2E_MOCK_COOKIE)?.value;
  if (!raw) return null;

  let mocks: Record<string, E2EMockEntry>;
  try {
    mocks = JSON.parse(decodeURIComponent(raw)) as Record<string, E2EMockEntry>;
  } catch {
    return null;
  }

  const entry = mocks[path];
  if (entry === undefined) return null;
  if (entry && typeof entry === "object" && !Array.isArray(entry) && "status" in entry) {
    const { status, body } = entry as { status: number; body?: unknown };
    if (status >= 400) throw new BackendError(status, JSON.stringify(body ?? { detail: "mocked error" }));
    return (body as T) ?? (undefined as T);
  }
  return entry as T;
}

export async function backend<T>(path: string, init: RequestInit = {}): Promise<T> {
  const mock = await readE2EMock<T>(path);
  if (mock !== null) return mock;
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
