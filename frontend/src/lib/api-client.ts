export async function client<T>(path: string, init: RequestInit = {}): Promise<T> {
  const res = await fetch(path, { ...init, headers: { "Content-Type": "application/json", ...init.headers } });
  if (!res.ok) throw new Error(`request failed: ${res.status}`);
  return res.status === 204 ? (undefined as T) : (res.json() as Promise<T>);
}
