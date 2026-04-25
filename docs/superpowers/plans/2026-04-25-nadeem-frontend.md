# Nadeem Web Frontend Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the Nadeem parental dashboard — an Arabic, RTL Next.js app that lets parents log in, see usage stats, browse the Semsem library, audit AI chat transcripts, manage devices, and add new devices via a one-time provisioning code.

**Architecture:** Next.js 15 App Router (React 19) + TypeScript + Tailwind CSS. Auth via JWT stored in an `httpOnly` cookie set by a Next.js Route Handler (so XSS can't steal the token); client components call same-origin Next.js API routes that forward to the Django backend. Design tokens (colors, fonts, mock screens) are already defined in `frontend/design.md` — port the Tailwind config from there verbatim and reuse the markup screen-by-screen as the starting HTML for each page. No state library; React Query for server cache.

**Tech Stack:** Next.js 15.x (App Router), React 19, TypeScript 5.x, Tailwind 3.x with `@tailwindcss/forms`, Material Symbols Outlined, fonts Baloo Bhaijaan 2 + Cairo, `@tanstack/react-query`, `zod` (form validation), `vitest` + `@testing-library/react` (unit), `playwright` (E2E). Docker-built; runs alongside the backend in compose.

**Out of scope:** Mobile companion app, signup flow (parent accounts created via Django admin / dev provisioning), localization beyond Arabic, dark-mode runtime toggle (design tokens include both, but ship light only).

**Backend dependency:** All endpoints in `2026-04-25-nadeem-backend.md`. Frontend cannot ship before tasks 3, 7, 10, 14, 15, 20 of the backend plan are merged.

---

## File Structure

```
frontend/
├── Dockerfile
├── docker-compose.yml                  # service block to be merged into backend's compose
├── package.json
├── tsconfig.json
├── next.config.ts
├── tailwind.config.ts
├── postcss.config.mjs
├── .env.example
├── .gitignore
├── vitest.config.ts
├── playwright.config.ts
├── public/
│   ├── nadeem-logo.svg                 # placeholder; design uses a hosted image, replace later
│   └── fonts/                          # if self-hosting (optional)
├── src/
│   ├── lib/
│   │   ├── api.ts                      # fetch wrapper (server-side)
│   │   ├── api-client.ts               # browser fetch wrapper (calls /api/*)
│   │   ├── jwt.ts                      # cookie helpers (server only)
│   │   ├── format.ts                   # ms→hours, dates in ar locale
│   │   └── query-client.tsx            # React Query Provider
│   ├── components/
│   │   ├── AppShell.tsx                # sidebar + topbar
│   │   ├── StatCard.tsx
│   │   ├── SemsemCard.tsx              # badge logic (عادي / ذكي)
│   │   ├── ChatBubble.tsx
│   │   ├── DeviceRow.tsx
│   │   ├── ConfirmDialog.tsx
│   │   ├── AddDeviceModal.tsx          # shows provision_token + expiry
│   │   ├── ErrorState.tsx
│   │   └── EmptyState.tsx
│   ├── app/
│   │   ├── layout.tsx                  # <html dir="rtl" lang="ar">
│   │   ├── globals.css
│   │   ├── login/page.tsx
│   │   ├── (dash)/                     # auth-protected route group
│   │   │   ├── layout.tsx              # AppShell wrapper, redirects to /login
│   │   │   ├── page.tsx                # main dashboard
│   │   │   ├── library/page.tsx
│   │   │   ├── library/[uid]/page.tsx
│   │   │   ├── chat-history/page.tsx
│   │   │   ├── chat-history/[id]/page.tsx
│   │   │   └── devices/page.tsx
│   │   └── api/
│   │       ├── auth/
│   │       │   ├── login/route.ts      # POST → backend; sets cookie
│   │       │   └── logout/route.ts     # DELETE → clears cookie
│   │       ├── me/route.ts             # proxies /api/auth/me
│   │       ├── stats/route.ts          # proxies /api/users/me/stats
│   │       ├── semsems/
│   │       │   ├── route.ts
│   │       │   └── [uid]/route.ts
│   │       ├── chat-sessions/
│   │       │   ├── route.ts
│   │       │   └── [id]/route.ts
│   │       ├── devices/
│   │       │   ├── route.ts
│   │       │   └── [id]/route.ts       # DELETE
│   │       └── provisioning-tokens/route.ts
│   └── middleware.ts                   # redirect unauth → /login
└── e2e/
    ├── login.spec.ts
    └── dashboard.spec.ts
```

**Design rules:**
- Every protected page is a server component that pre-fetches via the server-side `api.ts` (uses cookie); React Query hydrates on the client for refetch/mutations.
- The browser **never** holds a JWT in JS — it talks to `/api/*` Next.js routes that read the `httpOnly` cookie and forward to Django.
- All copy lives inline in Arabic (no i18n library); strings come straight from `frontend/design.md`. If a string is missing there, ask before inventing one.
- Tailwind config is **copied verbatim** from `frontend/design.md` — same palette, fonts, radii. Don't redesign.

---

### Task 1: Next.js scaffold + Tailwind + RTL + fonts

**Files:**
- Create everything listed under "File Structure" except the `app/` pages, components, and lib (those follow).

- [ ] **Step 1: `package.json`**

```json
{
  "name": "nadeem-frontend",
  "private": true,
  "scripts": {
    "dev": "next dev -H 0.0.0.0 -p 3000",
    "build": "next build",
    "start": "next start -H 0.0.0.0 -p 3000",
    "lint": "next lint",
    "test": "vitest run",
    "test:watch": "vitest",
    "e2e": "playwright test"
  },
  "dependencies": {
    "next": "^15.0.0",
    "react": "^19.0.0",
    "react-dom": "^19.0.0",
    "@tanstack/react-query": "^5.50.0",
    "zod": "^3.23.0"
  },
  "devDependencies": {
    "typescript": "^5.6.0",
    "@types/node": "^22",
    "@types/react": "^19",
    "@types/react-dom": "^19",
    "tailwindcss": "^3.4.0",
    "@tailwindcss/forms": "^0.5.0",
    "postcss": "^8.4.0",
    "autoprefixer": "^10.4.0",
    "vitest": "^2.0.0",
    "@testing-library/react": "^16.0.0",
    "@testing-library/jest-dom": "^6.4.0",
    "jsdom": "^25.0.0",
    "@playwright/test": "^1.46.0"
  }
}
```

- [ ] **Step 2: `tsconfig.json`** — Next.js default with `"baseUrl": "."`, `"paths": { "@/*": ["src/*"] }`, `"strict": true`.

- [ ] **Step 3: `next.config.ts`**

```ts
import type { NextConfig } from "next";

const config: NextConfig = {
  experimental: { typedRoutes: true },
  images: { remotePatterns: [{ protocol: "https", hostname: "**" }] },
};
export default config;
```

- [ ] **Step 4: `tailwind.config.ts`** — copy palette + fontFamily + borderRadius from `frontend/design.md` lines ~13-80 verbatim. Keep `darkMode: "class"`. Set `content: ["./src/**/*.{ts,tsx}"]`. Plugins: `[require("@tailwindcss/forms")]`.

```ts
import type { Config } from "tailwindcss";

const config: Config = {
  darkMode: "class",
  content: ["./src/**/*.{ts,tsx}"],
  theme: {
    extend: {
      colors: {
        // ↓ paste the entire colors object from frontend/design.md verbatim
      },
      borderRadius: { DEFAULT: "1rem", lg: "2rem", xl: "3rem", full: "9999px" },
      fontFamily: {
        headline: ["'Baloo Bhaijaan 2'", "sans-serif"],
        body: ["'Cairo'", "sans-serif"],
        label: ["'Cairo'", "sans-serif"],
      },
    },
  },
  plugins: [require("@tailwindcss/forms")],
};
export default config;
```

- [ ] **Step 5: `postcss.config.mjs`** — `export default { plugins: { tailwindcss: {}, autoprefixer: {} } };`.

- [ ] **Step 6: `src/app/globals.css`**

```css
@import url('https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400..800&family=Cairo:wght@400..800&family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&display=swap');

@tailwind base;
@tailwind components;
@tailwind utilities;

html, body { @apply bg-background text-on-background font-body min-h-screen; }
```

- [ ] **Step 7: `src/app/layout.tsx`**

```tsx
import "./globals.css";
import type { ReactNode } from "react";

export const metadata = { title: "نديم", description: "لوحة تحكم الآباء" };

export default function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html dir="rtl" lang="ar" className="light">
      <body>{children}</body>
    </html>
  );
}
```

- [ ] **Step 8: Placeholder root page** so `npm run dev` works:

`src/app/page.tsx`:

```tsx
export default function Home() {
  return <div className="p-8 font-headline text-2xl">نديم</div>;
}
```

- [ ] **Step 9: `Dockerfile`**

```dockerfile
FROM node:22-alpine
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY . .
EXPOSE 3000
CMD ["npm", "run", "dev"]
```

- [ ] **Step 10: `docker-compose.yml`** (frontend block to merge with backend's)

```yaml
services:
  frontend:
    build: ./frontend
    env_file: ./frontend/.env
    ports: ["3000:3000"]
    volumes: ["./frontend:/app", "/app/node_modules"]
    depends_on: [backend]
```

- [ ] **Step 11: `.env.example`**

```
NEXT_PUBLIC_APP_NAME=نديم
BACKEND_INTERNAL_URL=http://backend:8000
```

- [ ] **Step 12: `vitest.config.ts`**

```ts
import { defineConfig } from "vitest/config";
import path from "node:path";

export default defineConfig({
  test: { environment: "jsdom", globals: true, setupFiles: ["./vitest.setup.ts"] },
  resolve: { alias: { "@": path.resolve(__dirname, "src") } },
});
```

`vitest.setup.ts`: `import "@testing-library/jest-dom/vitest";`

- [ ] **Step 13: `playwright.config.ts`** (basic)

```ts
import { defineConfig } from "@playwright/test";
export default defineConfig({
  testDir: "./e2e",
  use: { baseURL: "http://localhost:3000", locale: "ar" },
});
```

- [ ] **Step 14: Smoke**

Run: `docker compose up -d frontend && curl -s http://localhost:3000` → returns HTML containing `dir="rtl"` and `نديم`.

- [ ] **Step 15: Commit** — `git add frontend/ && git commit -m "chore(frontend): scaffold Next.js + Tailwind + RTL + fonts"`

---

### Task 2: API client lib (server + browser) and JWT cookie

**Files:**
- Create: `src/lib/jwt.ts`, `src/lib/api.ts`, `src/lib/api-client.ts`, `src/lib/format.ts`, `src/lib/query-client.tsx`
- Create: `src/middleware.ts`

- [ ] **Step 1: `src/lib/jwt.ts`** (server-only — uses `next/headers`)

```ts
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
```

- [ ] **Step 2: `src/lib/api.ts`** (server-side fetch wrapper used by RSC + route handlers)

```ts
import { getAccessToken } from "./jwt";

const BASE = process.env.BACKEND_INTERNAL_URL!;

export async function backend<T>(path: string, init: RequestInit = {}): Promise<T> {
  const token = await getAccessToken();
  const headers = new Headers(init.headers);
  if (token) headers.set("Authorization", `Bearer ${token}`);
  if (!headers.has("Content-Type") && init.body) headers.set("Content-Type", "application/json");
  const res = await fetch(`${BASE}${path}`, { ...init, headers, cache: "no-store" });
  if (!res.ok) {
    throw new BackendError(res.status, await res.text());
  }
  return res.status === 204 ? (undefined as T) : (res.json() as Promise<T>);
}

export class BackendError extends Error {
  constructor(public status: number, body: string) {
    super(`backend ${status}: ${body}`);
  }
}
```

- [ ] **Step 3: `src/lib/api-client.ts`** (browser — calls Next.js API routes, never the backend directly)

```ts
export async function client<T>(path: string, init: RequestInit = {}): Promise<T> {
  const res = await fetch(path, { ...init, headers: { "Content-Type": "application/json", ...init.headers } });
  if (!res.ok) throw new Error(`request failed: ${res.status}`);
  return res.status === 204 ? (undefined as T) : (res.json() as Promise<T>);
}
```

- [ ] **Step 4: `src/lib/format.ts`**

```ts
export function msToHours(ms: number): string {
  const h = ms / 3_600_000;
  return new Intl.NumberFormat("ar", { maximumFractionDigits: 1 }).format(h);
}

export function msToMinutes(ms: number): string {
  return new Intl.NumberFormat("ar", { maximumFractionDigits: 0 }).format(ms / 60_000);
}

export function fmtDate(iso: string): string {
  return new Intl.DateTimeFormat("ar", { dateStyle: "long", timeStyle: "short" }).format(new Date(iso));
}
```

- [ ] **Step 5: `src/lib/query-client.tsx`**

```tsx
"use client";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { useState, type ReactNode } from "react";

export function QueryProvider({ children }: { children: ReactNode }) {
  const [client] = useState(() => new QueryClient({ defaultOptions: { queries: { staleTime: 30_000, refetchOnWindowFocus: false } } }));
  return <QueryClientProvider client={client}>{children}</QueryClientProvider>;
}
```

- [ ] **Step 6: `src/middleware.ts`** (redirect unauth → /login except for /login + /api/auth/login)

```ts
import { NextRequest, NextResponse } from "next/server";

const PUBLIC = ["/login", "/api/auth/login"];

export function middleware(req: NextRequest) {
  const url = req.nextUrl;
  if (PUBLIC.some((p) => url.pathname === p || url.pathname.startsWith(p + "/"))) return NextResponse.next();
  const token = req.cookies.get("nadeem_jwt")?.value;
  if (!token && !url.pathname.startsWith("/api")) {
    const r = url.clone();
    r.pathname = "/login";
    return NextResponse.redirect(r);
  }
  if (!token && url.pathname.startsWith("/api")) {
    return NextResponse.json({ detail: "unauthorized" }, { status: 401 });
  }
  return NextResponse.next();
}

export const config = { matcher: ["/((?!_next/static|_next/image|favicon.ico).*)"] };
```

- [ ] **Step 7: Test format helpers (vitest)**

`src/lib/__tests__/format.test.ts`:

```ts
import { describe, it, expect } from "vitest";
import { msToHours, msToMinutes } from "../format";

describe("format", () => {
  it("converts ms to hours with arabic numerals locale", () => {
    expect(typeof msToHours(3_600_000)).toBe("string");
  });
  it("rounds minutes", () => {
    expect(msToMinutes(120_000)).toMatch(/2/);
  });
});
```

Run: `npm run test` → 2 passed.

- [ ] **Step 8: Commit** — `git commit -m "feat(frontend): api client + jwt cookie + middleware"`

---

### Task 3: `POST /api/auth/login` route + login page

**Files:**
- Create: `src/app/api/auth/login/route.ts`
- Create: `src/app/api/auth/logout/route.ts`
- Create: `src/app/login/page.tsx`
- Create: `e2e/login.spec.ts`

- [ ] **Step 1: `src/app/api/auth/login/route.ts`**

```ts
import { NextRequest, NextResponse } from "next/server";
import { setJwtCookie } from "@/lib/jwt";

export async function POST(req: NextRequest) {
  const body = await req.json();
  const r = await fetch(`${process.env.BACKEND_INTERNAL_URL}/api/auth/login`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });
  if (!r.ok) return NextResponse.json({ detail: "invalid credentials" }, { status: 401 });
  const { access, refresh } = await r.json();
  await setJwtCookie(access, refresh);
  return NextResponse.json({ ok: true });
}
```

- [ ] **Step 2: `src/app/api/auth/logout/route.ts`**

```ts
import { NextResponse } from "next/server";
import { clearJwtCookie } from "@/lib/jwt";

export async function DELETE() {
  await clearJwtCookie();
  return new NextResponse(null, { status: 204 });
}
```

- [ ] **Step 3: `src/app/login/page.tsx`**

Use the Login Page markup from `frontend/design.md` lines 1-90 (the first `<!-- Login Page -->` block). Convert to a React component, replace `<form>` with controlled state, add `onSubmit`:

```tsx
"use client";
import { useState } from "react";
import { useRouter } from "next/navigation";

export default function LoginPage() {
  const router = useRouter();
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [err, setErr] = useState<string | null>(null);
  const [busy, setBusy] = useState(false);

  async function submit(e: React.FormEvent) {
    e.preventDefault();
    setErr(null); setBusy(true);
    try {
      const r = await fetch("/api/auth/login", {
        method: "POST", headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email, password }),
      });
      if (!r.ok) throw new Error();
      router.push("/");
    } catch {
      setErr("بيانات تسجيل الدخول غير صحيحة");
    } finally { setBusy(false); }
  }

  return (
    <main className="bg-background text-on-background font-body min-h-screen flex items-center justify-center relative overflow-hidden">
      {/* Decorative blobs from design.md */}
      <div className="absolute top-[-10%] right-[-5%] w-96 h-96 bg-primary-container rounded-full mix-blend-multiply filter blur-3xl opacity-30 z-0" />
      <div className="absolute bottom-[-10%] left-[-5%] w-80 h-80 bg-secondary-container rounded-full mix-blend-multiply filter blur-3xl opacity-30 z-0" />

      <div className="relative z-10 w-full max-w-md px-6 py-12">
        <div className="bg-surface-container-highest rounded-xl p-10 shadow-[0_8px_30px_rgb(0,0,0,0.04)]">
          <div className="text-center mb-10">
            <h1 className="font-headline text-4xl font-bold text-primary">نديم</h1>
            <p className="font-body text-on-surface-variant mt-2 text-lg">مرحباً بك في عالم الخيال</p>
          </div>
          <form className="space-y-6" onSubmit={submit}>
            <div>
              <label className="block font-label text-sm font-semibold text-on-surface mb-2" htmlFor="email">البريد الإلكتروني</label>
              <input id="email" type="email" autoComplete="email" required value={email} onChange={(e) => setEmail(e.target.value)}
                     className="block w-full rounded-md border-0 py-3 px-4 text-on-surface ring-1 ring-inset ring-outline-variant focus:ring-2 focus:ring-primary bg-surface-container-lowest font-body" />
            </div>
            <div>
              <label className="block font-label text-sm font-semibold text-on-surface mb-2" htmlFor="password">كلمة المرور</label>
              <input id="password" type="password" autoComplete="current-password" required value={password} onChange={(e) => setPassword(e.target.value)}
                     className="block w-full rounded-md border-0 py-3 px-4 text-on-surface ring-1 ring-inset ring-outline-variant focus:ring-2 focus:ring-primary bg-surface-container-lowest font-body" />
            </div>
            {err && <p className="text-error font-body text-sm">{err}</p>}
            <button disabled={busy} type="submit"
                    className="w-full flex justify-center py-4 px-4 rounded-full text-lg font-headline font-bold text-on-primary bg-gradient-to-br from-primary to-primary-container hover:scale-[1.02] transition-transform disabled:opacity-60">
              {busy ? "..." : "تسجيل الدخول"}
            </button>
          </form>
        </div>
      </div>
    </main>
  );
}
```

- [ ] **Step 4: E2E test `e2e/login.spec.ts`**

```ts
import { test, expect } from "@playwright/test";

test("redirects unauth user to /login", async ({ page }) => {
  await page.goto("/");
  await expect(page).toHaveURL(/\/login$/);
});

test("login with valid creds → dashboard", async ({ page }) => {
  await page.goto("/login");
  await page.getByLabel("البريد الإلكتروني").fill("parent@example.com");
  await page.getByLabel("كلمة المرور").fill("hunter2");
  await page.getByRole("button", { name: "تسجيل الدخول" }).click();
  await expect(page).toHaveURL("/");
});
```

- [ ] **Step 5: Smoke: register a parent in backend (`provision_dev_device` requires a user; `manage.py createsuperuser`), then visit /login manually.**

- [ ] **Step 6: Commit** — `git commit -m "feat(frontend): login page + jwt cookie route"`

---

### Task 4: App shell layout (sidebar + topbar) for protected pages

**Files:**
- Create: `src/components/AppShell.tsx`
- Create: `src/app/(dash)/layout.tsx`

Use the dashboard mockup from `frontend/design.md` (the second `<!-- Dashboard Home -->` block, sidebar markup); refactor sidebar nav into `AppShell`. Protected layout server-fetches the current user via `/api/auth/me` and passes their email to the shell.

- [ ] **Step 1: `src/app/(dash)/layout.tsx`**

```tsx
import { redirect } from "next/navigation";
import { backend, BackendError } from "@/lib/api";
import { AppShell } from "@/components/AppShell";
import { QueryProvider } from "@/lib/query-client";

type Me = { id: number; email: string };

export default async function DashLayout({ children }: { children: React.ReactNode }) {
  let me: Me;
  try {
    me = await backend<Me>("/api/auth/me");
  } catch (e) {
    if (e instanceof BackendError && e.status === 401) redirect("/login");
    throw e;
  }
  return (
    <QueryProvider>
      <AppShell email={me.email}>{children}</AppShell>
    </QueryProvider>
  );
}
```

- [ ] **Step 2: `src/components/AppShell.tsx`**

Port the sidebar markup verbatim from `frontend/design.md` Dashboard mockup (top sidebar with logo + nav items: الرئيسية, مكتبة سمسم, محادثات سمسم الذكي, الأجهزة, الإعدادات). Use `next/link` for nav. Add a logout button that `DELETE`s `/api/auth/logout` and pushes `/login`.

```tsx
"use client";
import Link from "next/link";
import { usePathname, useRouter } from "next/navigation";
import type { ReactNode } from "react";

const NAV = [
  { href: "/", label: "الرئيسية", icon: "home" },
  { href: "/library", label: "مكتبة سمسم", icon: "library_music" },
  { href: "/chat-history", label: "محادثات سمسم الذكي", icon: "smart_toy" },
  { href: "/devices", label: "الأجهزة", icon: "devices" },
];

export function AppShell({ email, children }: { email: string; children: ReactNode }) {
  const path = usePathname();
  const router = useRouter();
  async function logout() {
    await fetch("/api/auth/logout", { method: "DELETE" });
    router.push("/login");
  }
  return (
    <div className="min-h-screen bg-background text-on-background flex">
      <aside className="w-64 bg-surface-container-low p-6 flex flex-col gap-2">
        <h1 className="font-headline text-3xl font-bold text-primary mb-8">نديم</h1>
        <nav className="flex flex-col gap-1">
          {NAV.map((n) => {
            const active = path === n.href || (n.href !== "/" && path.startsWith(n.href));
            return (
              <Link key={n.href} href={n.href}
                className={`flex items-center gap-3 px-4 py-3 rounded-lg font-body ${active ? "bg-primary-container text-on-primary-container font-bold" : "hover:bg-surface-container"}`}>
                <span className="material-symbols-outlined">{n.icon}</span>
                {n.label}
              </Link>
            );
          })}
        </nav>
        <div className="mt-auto pt-6 border-t border-outline-variant">
          <p className="text-sm text-on-surface-variant truncate">{email}</p>
          <button onClick={logout} className="mt-2 text-sm text-primary hover:underline">تسجيل الخروج</button>
        </div>
      </aside>
      <main className="flex-1 p-10">{children}</main>
    </div>
  );
}
```

- [ ] **Step 3: `/api/me` proxy** — `src/app/api/me/route.ts`

```ts
import { NextResponse } from "next/server";
import { backend } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/auth/me")); }
  catch { return NextResponse.json({ detail: "unauthorized" }, { status: 401 }); }
}
```

- [ ] **Step 4: Smoke**: visit `/` after login → see sidebar with nav, email, logout. Click logout → back to `/login`.

- [ ] **Step 5: Commit** — `git commit -m "feat(frontend): protected layout + AppShell with logout"`

---

### Task 5: Dashboard home page (stats + devices)

**Files:**
- Create: `src/app/(dash)/page.tsx`
- Create: `src/components/StatCard.tsx`
- Create: `src/components/DeviceRow.tsx`
- Create: `src/app/api/stats/route.ts`
- Create: `src/app/api/devices/route.ts`

- [ ] **Step 1: API proxy routes**

`src/app/api/stats/route.ts`:

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/users/me/stats")); }
  catch (e) {
    const s = e instanceof BackendError ? e.status : 500;
    return NextResponse.json({ detail: "error" }, { status: s });
  }
}
```

`src/app/api/devices/route.ts` (GET):

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/devices")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `StatCard`** (port a card from the design)

```tsx
import type { ReactNode } from "react";

export function StatCard({ icon, label, value, sub }: { icon: string; label: string; value: ReactNode; sub?: ReactNode }) {
  return (
    <div className="bg-surface-container-highest rounded-xl p-6">
      <div className="flex items-center gap-3 text-on-surface-variant mb-2">
        <span className="material-symbols-outlined">{icon}</span>
        <span className="font-label text-sm">{label}</span>
      </div>
      <div className="font-headline text-4xl font-bold text-on-surface">{value}</div>
      {sub && <div className="font-body text-sm text-on-surface-variant mt-1">{sub}</div>}
    </div>
  );
}
```

- [ ] **Step 3: `DeviceRow`**

```tsx
type Device = { device_id: string; online: boolean; last_seen_at: string | null };

export function DeviceRow({ d }: { d: Device }) {
  return (
    <div className="flex items-center justify-between py-3 border-b border-outline-variant">
      <div>
        <div className="font-body text-on-surface">{d.device_id}</div>
        <div className="text-sm text-on-surface-variant">{d.last_seen_at ? `آخر اتصال: ${new Date(d.last_seen_at).toLocaleString("ar")}` : "لم يتصل بعد"}</div>
      </div>
      <span className={`px-3 py-1 rounded-full text-xs font-bold ${d.online ? "bg-primary-container text-on-primary-container" : "bg-surface-container text-on-surface-variant"}`}>
        {d.online ? "متصل" : "غير متصل"}
      </span>
    </div>
  );
}
```

- [ ] **Step 4: `src/app/(dash)/page.tsx`** (server component, prefetch stats + devices)

```tsx
import { backend } from "@/lib/api";
import { StatCard } from "@/components/StatCard";
import { DeviceRow } from "@/components/DeviceRow";
import { msToHours, msToMinutes } from "@/lib/format";

type Stats = { total_listening_ms: number; unique_semsems: number; pro_total_ms: number; device_count: number; online_device_count: number };
type Device = { device_id: string; online: boolean; last_seen_at: string | null; created_at: string };

export default async function Dashboard() {
  const [stats, devices] = await Promise.all([backend<Stats>("/api/users/me/stats"), backend<Device[]>("/api/devices")]);
  return (
    <div className="space-y-8">
      <h2 className="font-headline text-3xl font-bold text-on-surface">الرئيسية</h2>
      <div className="grid grid-cols-1 md:grid-cols-4 gap-6">
        <StatCard icon="schedule" label="ساعات الاستماع" value={msToHours(stats.total_listening_ms)} sub="إجمالي" />
        <StatCard icon="album" label="السمسمات" value={stats.unique_semsems} sub="فريدة" />
        <StatCard icon="smart_toy" label="دقائق المحادثة الذكية" value={msToMinutes(stats.pro_total_ms)} />
        <StatCard icon="devices" label="الأجهزة" value={`${stats.online_device_count} / ${stats.device_count}`} sub="المتصل / الإجمالي" />
      </div>
      <section className="bg-surface-container-highest rounded-xl p-6">
        <h3 className="font-headline text-xl font-bold mb-4">أجهزتي</h3>
        {devices.length === 0 ? <p className="text-on-surface-variant">لا توجد أجهزة بعد</p> :
          devices.map((d) => <DeviceRow key={d.device_id} d={d} />)}
      </section>
    </div>
  );
}
```

- [ ] **Step 5: Smoke**: with `provision_dev_device` having seeded one device + a few `UsageStats` rows, dashboard shows non-zero numbers.

- [ ] **Step 6: Commit** — `git commit -m "feat(frontend): dashboard with stats + device list"`

---

### Task 6: Library list page

**Files:**
- Create: `src/app/(dash)/library/page.tsx`
- Create: `src/components/SemsemCard.tsx`
- Create: `src/app/api/semsems/route.ts`

- [ ] **Step 1: Proxy** — `src/app/api/semsems/route.ts`

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/semsems")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `SemsemCard`** (badge logic per PRD §5)

```tsx
import Link from "next/link";

type Semsem = { uid_hex: string; title: string; is_pro: boolean; role: string };

export function SemsemCard({ s }: { s: Semsem }) {
  return (
    <Link href={{ pathname: "/library/[uid]", query: { uid: s.uid_hex } }}
          className="block bg-surface-container-highest rounded-xl p-6 hover:scale-[1.01] transition-transform">
      <div className="flex justify-between items-start mb-3">
        <h3 className="font-headline text-xl font-bold text-on-surface">{s.title}</h3>
        <span className={`px-3 py-1 rounded-full text-xs font-bold ${s.is_pro ? "bg-secondary-container text-on-secondary-container" : "bg-primary-container text-on-primary-container"}`}>
          {s.is_pro ? "ذكي" : "عادي"}
        </span>
      </div>
      <p className="font-body text-sm text-on-surface-variant">UID: {s.uid_hex}</p>
      {s.is_pro && <p className="font-body text-sm text-on-surface-variant">دور: {s.role}</p>}
    </Link>
  );
}
```

- [ ] **Step 3: Page**

```tsx
import { backend } from "@/lib/api";
import { SemsemCard } from "@/components/SemsemCard";

export default async function Library() {
  const list = await backend<{ uid_hex: string; title: string; is_pro: boolean; role: string }[]>("/api/semsems");
  return (
    <div className="space-y-6">
      <h2 className="font-headline text-3xl font-bold text-on-surface">مكتبة سمسم</h2>
      {list.length === 0
        ? <p className="text-on-surface-variant">لم يتم استخدام أي سمسم بعد</p>
        : <div className="grid grid-cols-1 md:grid-cols-3 gap-6">{list.map((s) => <SemsemCard key={s.uid_hex} s={s} />)}</div>}
    </div>
  );
}
```

- [ ] **Step 4: Component test**

`src/components/__tests__/SemsemCard.test.tsx`:

```tsx
import { render, screen } from "@testing-library/react";
import { describe, it, expect } from "vitest";
import { SemsemCard } from "../SemsemCard";

describe("SemsemCard", () => {
  it("shows ذكي badge for pro", () => {
    render(<SemsemCard s={{ uid_hex: "aa", title: "x", is_pro: true, role: "doctor" }} />);
    expect(screen.getByText("ذكي")).toBeInTheDocument();
  });
  it("shows عادي badge for regular", () => {
    render(<SemsemCard s={{ uid_hex: "aa", title: "x", is_pro: false, role: "" }} />);
    expect(screen.getByText("عادي")).toBeInTheDocument();
  });
});
```

Run: `npm run test`.

- [ ] **Step 5: Commit** — `git commit -m "feat(frontend): library list with pro/regular badges"`

---

### Task 7: Library detail page

**Files:**
- Create: `src/app/(dash)/library/[uid]/page.tsx`
- Create: `src/app/api/semsems/[uid]/route.ts`

- [ ] **Step 1: Proxy**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET(_: Request, ctx: { params: Promise<{ uid: string }> }) {
  const { uid } = await ctx.params;
  try { return NextResponse.json(await backend(`/api/semsems/${uid}`)); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: Page**

```tsx
import { backend } from "@/lib/api";
import { msToMinutes } from "@/lib/format";

type Detail = {
  uid_hex: string; title: string; is_pro: boolean; role: string;
  tracks: { name: string; url: string }[];
  play_count: number; total_play_ms: number; pro_session_count: number; pro_total_ms: number;
};

export default async function SemsemDetail({ params }: { params: Promise<{ uid: string }> }) {
  const { uid } = await params;
  const d = await backend<Detail>(`/api/semsems/${uid}`);
  return (
    <div className="space-y-6">
      <header>
        <span className={`inline-block px-3 py-1 rounded-full text-xs font-bold mb-3 ${d.is_pro ? "bg-secondary-container text-on-secondary-container" : "bg-primary-container text-on-primary-container"}`}>
          {d.is_pro ? "ذكي" : "عادي"}
        </span>
        <h2 className="font-headline text-3xl font-bold text-on-surface">{d.title}</h2>
        <p className="text-on-surface-variant">UID: {d.uid_hex}{d.is_pro && ` · دور: ${d.role}`}</p>
      </header>
      <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
        <div className="bg-surface-container-highest rounded-xl p-6">
          <h3 className="font-headline text-xl font-bold mb-3">إحصائيات</h3>
          <ul className="space-y-2 font-body">
            <li>عدد التشغيلات: {d.play_count}</li>
            <li>إجمالي وقت التشغيل: {msToMinutes(d.total_play_ms)} دقيقة</li>
            {d.is_pro && <>
              <li>جلسات ذكية: {d.pro_session_count}</li>
              <li>إجمالي وقت المحادثة: {msToMinutes(d.pro_total_ms)} دقيقة</li>
            </>}
          </ul>
        </div>
        {!d.is_pro && (
          <div className="bg-surface-container-highest rounded-xl p-6">
            <h3 className="font-headline text-xl font-bold mb-3">المقاطع الصوتية</h3>
            <ul className="space-y-2 font-body">
              {d.tracks.map((t) => (
                <li key={t.name} className="flex items-center gap-3">
                  <span className="material-symbols-outlined text-primary">play_circle</span>
                  <a href={t.url} className="hover:underline" target="_blank" rel="noreferrer">{t.name}</a>
                </li>
              ))}
            </ul>
          </div>
        )}
      </div>
    </div>
  );
}
```

- [ ] **Step 3: Smoke**: navigate from library list → detail page; numbers and tracks render.

- [ ] **Step 4: Commit** — `git commit -m "feat(frontend): library detail page"`

---

### Task 8: Chat history list

**Files:**
- Create: `src/app/(dash)/chat-history/page.tsx`
- Create: `src/app/api/chat-sessions/route.ts`

- [ ] **Step 1: Proxy** (analogous to `/api/semsems/route.ts`).

- [ ] **Step 2: Page**

```tsx
import Link from "next/link";
import { backend } from "@/lib/api";
import { fmtDate } from "@/lib/format";

type S = { id: number; device_id: string; uid_hex: string; role: string; started_at: string; ended_at: string | null };

export default async function History() {
  const sessions = await backend<S[]>("/api/chat-sessions");
  return (
    <div className="space-y-6">
      <h2 className="font-headline text-3xl font-bold text-on-surface">محادثات سمسم الذكي</h2>
      {sessions.length === 0
        ? <p className="text-on-surface-variant">لا توجد محادثات بعد</p>
        : <ul className="space-y-3">
            {sessions.map((s) => (
              <li key={s.id}>
                <Link href={{ pathname: "/chat-history/[id]", query: { id: s.id.toString() } }}
                      className="block bg-surface-container-highest rounded-xl p-5 hover:scale-[1.01] transition-transform">
                  <div className="flex justify-between items-center">
                    <div>
                      <div className="font-headline text-lg font-bold">دور: {s.role}</div>
                      <div className="text-sm text-on-surface-variant">UID: {s.uid_hex} · جهاز: {s.device_id}</div>
                    </div>
                    <div className="text-sm text-on-surface-variant">{fmtDate(s.started_at)}</div>
                  </div>
                </Link>
              </li>
            ))}
          </ul>}
    </div>
  );
}
```

- [ ] **Step 3: Commit** — `git commit -m "feat(frontend): chat history list"`

---

### Task 9: Chat detail (transcript with bubbles)

**Files:**
- Create: `src/app/(dash)/chat-history/[id]/page.tsx`
- Create: `src/app/api/chat-sessions/[id]/route.ts`
- Create: `src/components/ChatBubble.tsx`
- Create: `src/components/__tests__/ChatBubble.test.tsx`

- [ ] **Step 1: Proxy** (analogous to library detail proxy, route param `id`).

- [ ] **Step 2: `ChatBubble`**

```tsx
type Props = { speaker: "child" | "ai"; text: string; timestamp: string };

export function ChatBubble({ speaker, text, timestamp }: Props) {
  const mine = speaker === "child";
  return (
    <div className={`flex ${mine ? "justify-start" : "justify-end"}`}>
      <div className={`max-w-[75%] rounded-xl p-4 font-body
        ${mine ? "bg-primary-container text-on-primary-container rounded-bl-none" : "bg-secondary-container text-on-secondary-container rounded-br-none"}`}>
        <div className="text-xs font-bold mb-1">{mine ? "الطفل" : "نديم"}</div>
        <div>{text}</div>
        <div className="text-[10px] mt-1 opacity-70">{new Date(timestamp).toLocaleTimeString("ar")}</div>
      </div>
    </div>
  );
}
```

- [ ] **Step 3: Bubble test**

```tsx
import { render, screen } from "@testing-library/react";
import { describe, it, expect } from "vitest";
import { ChatBubble } from "../ChatBubble";

describe("ChatBubble", () => {
  it("renders child label for speaker=child", () => {
    render(<ChatBubble speaker="child" text="hi" timestamp={new Date().toISOString()} />);
    expect(screen.getByText("الطفل")).toBeInTheDocument();
  });
  it("renders nadeem label for speaker=ai", () => {
    render(<ChatBubble speaker="ai" text="hi" timestamp={new Date().toISOString()} />);
    expect(screen.getByText("نديم")).toBeInTheDocument();
  });
});
```

- [ ] **Step 4: Page**

```tsx
import { backend } from "@/lib/api";
import { ChatBubble } from "@/components/ChatBubble";
import { fmtDate } from "@/lib/format";

type Detail = {
  id: number; device_id: string; uid_hex: string; role: string;
  started_at: string; ended_at: string | null;
  entries: { speaker: "child" | "ai"; text: string; timestamp: string }[];
};

export default async function ChatDetail({ params }: { params: Promise<{ id: string }> }) {
  const { id } = await params;
  const s = await backend<Detail>(`/api/chat-sessions/${id}`);
  return (
    <div className="space-y-6">
      <header>
        <h2 className="font-headline text-3xl font-bold text-on-surface">دور: {s.role}</h2>
        <p className="text-on-surface-variant">{fmtDate(s.started_at)} · جهاز: {s.device_id}</p>
      </header>
      <div className="bg-surface-container rounded-xl p-6 space-y-3">
        {s.entries.length === 0 ? <p className="text-on-surface-variant">لا توجد رسائل في هذه الجلسة</p> :
          s.entries.map((e, i) => <ChatBubble key={i} {...e} />)}
      </div>
    </div>
  );
}
```

- [ ] **Step 5: Run vitest, commit** — `git commit -m "feat(frontend): chat detail with bubble layout"`

---

### Task 10: Devices page (list + revoke)

**Files:**
- Create: `src/app/(dash)/devices/page.tsx`
- Create: `src/app/api/devices/[id]/route.ts`
- Create: `src/components/ConfirmDialog.tsx`

- [ ] **Step 1: Proxy DELETE** — `src/app/api/devices/[id]/route.ts`

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function DELETE(_: Request, ctx: { params: Promise<{ id: string }> }) {
  const { id } = await ctx.params;
  try { await backend(`/api/devices/${id}`, { method: "DELETE" }); return new NextResponse(null, { status: 204 }); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `ConfirmDialog`** (simple controlled `<dialog>` based)

```tsx
"use client";
import { useEffect, useRef } from "react";

type Props = { open: boolean; title: string; body: string; confirmLabel: string; onConfirm: () => void; onClose: () => void };

export function ConfirmDialog({ open, title, body, confirmLabel, onConfirm, onClose }: Props) {
  const ref = useRef<HTMLDialogElement>(null);
  useEffect(() => { if (open) ref.current?.showModal(); else ref.current?.close(); }, [open]);
  return (
    <dialog ref={ref} onClose={onClose} className="rounded-xl p-0 bg-surface-container-highest">
      <div className="p-6 max-w-md">
        <h3 className="font-headline text-xl font-bold mb-2">{title}</h3>
        <p className="font-body text-on-surface-variant mb-6">{body}</p>
        <div className="flex gap-3 justify-end">
          <button onClick={onClose} className="px-4 py-2 rounded-full bg-surface-container">إلغاء</button>
          <button onClick={onConfirm} className="px-4 py-2 rounded-full bg-error text-on-error font-bold">{confirmLabel}</button>
        </div>
      </div>
    </dialog>
  );
}
```

- [ ] **Step 3: Devices page** (client component, uses React Query)

```tsx
"use client";
import { useState } from "react";
import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { DeviceRow } from "@/components/DeviceRow";
import { ConfirmDialog } from "@/components/ConfirmDialog";
import { AddDeviceModal } from "@/components/AddDeviceModal";

type Device = { device_id: string; online: boolean; last_seen_at: string | null; created_at: string };

export default function DevicesPage() {
  const qc = useQueryClient();
  const { data = [], isLoading } = useQuery({ queryKey: ["devices"], queryFn: () => client<Device[]>("/api/devices") });
  const remove = useMutation({
    mutationFn: (id: string) => client(`/api/devices/${id}`, { method: "DELETE" }),
    onSuccess: () => qc.invalidateQueries({ queryKey: ["devices"] }),
  });
  const [confirming, setConfirming] = useState<string | null>(null);
  const [adding, setAdding] = useState(false);

  return (
    <div className="space-y-6">
      <div className="flex justify-between items-center">
        <h2 className="font-headline text-3xl font-bold text-on-surface">الأجهزة</h2>
        <button onClick={() => setAdding(true)} className="px-5 py-3 rounded-full bg-primary text-on-primary font-bold">إضافة جهاز</button>
      </div>
      {isLoading ? <p>...</p> : data.length === 0 ? <p className="text-on-surface-variant">لا توجد أجهزة</p> :
        <div className="bg-surface-container-highest rounded-xl p-6 space-y-2">
          {data.map((d) => (
            <div key={d.device_id} className="flex items-center justify-between border-b border-outline-variant py-3">
              <div className="flex-1"><DeviceRow d={d} /></div>
              <button onClick={() => setConfirming(d.device_id)} className="px-4 py-2 rounded-full text-error hover:bg-error-container">إلغاء التفعيل</button>
            </div>
          ))}
        </div>}

      <ConfirmDialog open={!!confirming} title="إلغاء تفعيل الجهاز"
        body="سيتم إبطال رمز الجهاز فوراً. لن يستطيع الجهاز الوصول إلى الخادم حتى تتم إعادة الإقران."
        confirmLabel="نعم، ألغِ التفعيل"
        onConfirm={() => { if (confirming) { remove.mutate(confirming); setConfirming(null); } }}
        onClose={() => setConfirming(null)} />

      <AddDeviceModal open={adding} onClose={() => setAdding(false)} />
    </div>
  );
}
```

- [ ] **Step 4: Commit** — `git commit -m "feat(frontend): devices page + revoke confirm dialog"`

---

### Task 11: Add device flow (provisioning token modal)

PRD §3.1: parent triggers `POST /api/provisioning-tokens`, gets a single-use 10-min token. Until the mobile app exists, the modal just **displays** the token + expiry; the user runs the dev provisioning command (or pastes the token into a future companion app) to bind.

**Files:**
- Create: `src/components/AddDeviceModal.tsx`
- Create: `src/app/api/provisioning-tokens/route.ts`

- [ ] **Step 1: Proxy POST**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function POST() {
  try { return NextResponse.json(await backend("/api/provisioning-tokens", { method: "POST", body: JSON.stringify({}) }), { status: 201 }); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `AddDeviceModal`**

```tsx
"use client";
import { useEffect, useRef, useState } from "react";

type Resp = { provision_token: string; expires_at: string };

export function AddDeviceModal({ open, onClose }: { open: boolean; onClose: () => void }) {
  const ref = useRef<HTMLDialogElement>(null);
  const [data, setData] = useState<Resp | null>(null);
  const [err, setErr] = useState<string | null>(null);
  const [busy, setBusy] = useState(false);

  useEffect(() => { if (open) { ref.current?.showModal(); setData(null); setErr(null); } else ref.current?.close(); }, [open]);

  async function generate() {
    setBusy(true); setErr(null);
    try {
      const r = await fetch("/api/provisioning-tokens", { method: "POST" });
      if (!r.ok) throw new Error();
      setData(await r.json());
    } catch { setErr("تعذر إنشاء الرمز"); } finally { setBusy(false); }
  }

  return (
    <dialog ref={ref} onClose={onClose} className="rounded-xl p-0 bg-surface-container-highest">
      <div className="p-6 max-w-md">
        <h3 className="font-headline text-xl font-bold mb-3">إضافة جهاز</h3>
        {!data ? (
          <>
            <p className="font-body text-on-surface-variant mb-4">سيتم إنشاء رمز إقران صالح لمدة عشر دقائق فقط. أدخله في تطبيق الإعداد على هاتفك.</p>
            <div className="flex justify-end gap-3">
              <button onClick={onClose} className="px-4 py-2 rounded-full bg-surface-container">إلغاء</button>
              <button onClick={generate} disabled={busy} className="px-4 py-2 rounded-full bg-primary text-on-primary font-bold disabled:opacity-60">{busy ? "..." : "إنشاء الرمز"}</button>
            </div>
            {err && <p className="text-error mt-3">{err}</p>}
          </>
        ) : (
          <>
            <p className="font-body text-on-surface-variant mb-2">رمز الإقران (صالح حتى {new Date(data.expires_at).toLocaleString("ar")}):</p>
            <code className="block bg-surface-container p-3 rounded font-mono text-sm break-all">{data.provision_token}</code>
            <div className="flex justify-end mt-5">
              <button onClick={onClose} className="px-4 py-2 rounded-full bg-primary text-on-primary font-bold">تم</button>
            </div>
          </>
        )}
      </div>
    </dialog>
  );
}
```

- [ ] **Step 3: Smoke**: open Devices page → click "إضافة جهاز" → click "إنشاء الرمز" → token + expiry render.

- [ ] **Step 4: Commit** — `git commit -m "feat(frontend): add device modal generates provisioning token"`

---

### Task 12: E2E happy-path + dashboard E2E

**Files:**
- Create: `e2e/dashboard.spec.ts`

- [ ] **Step 1: Test**

```ts
import { test, expect } from "@playwright/test";

test.describe("dashboard e2e", () => {
  test.beforeEach(async ({ page }) => {
    await page.goto("/login");
    await page.getByLabel("البريد الإلكتروني").fill("parent@example.com");
    await page.getByLabel("كلمة المرور").fill("hunter2");
    await page.getByRole("button", { name: "تسجيل الدخول" }).click();
    await page.waitForURL("/");
  });

  test("nav links work", async ({ page }) => {
    await page.getByRole("link", { name: /مكتبة سمسم/ }).click();
    await expect(page).toHaveURL("/library");
    await page.getByRole("link", { name: /محادثات سمسم الذكي/ }).click();
    await expect(page).toHaveURL("/chat-history");
    await page.getByRole("link", { name: /الأجهزة/ }).click();
    await expect(page).toHaveURL("/devices");
  });

  test("add device generates token", async ({ page }) => {
    await page.goto("/devices");
    await page.getByRole("button", { name: "إضافة جهاز" }).click();
    await page.getByRole("button", { name: "إنشاء الرمز" }).click();
    await expect(page.getByText(/صالح حتى/)).toBeVisible();
  });
});
```

- [ ] **Step 2: Run** — bring up backend + frontend, seed a parent + a few rows, then `npm run e2e`. Inspect failures, fix selectors as needed.

- [ ] **Step 3: Commit** — `git commit -m "test(frontend): playwright e2e for nav + add-device"`

---

### Task 13: Production polish (loading, errors, empty states)

**Files:**
- Modify: every page added so far
- Create: `src/components/ErrorState.tsx`, `src/components/EmptyState.tsx`
- Create: `src/app/(dash)/error.tsx`, `src/app/(dash)/loading.tsx`

- [ ] **Step 1: Reusable `EmptyState` and `ErrorState`**

```tsx
// EmptyState.tsx
export function EmptyState({ icon, title, body }: { icon: string; title: string; body: string }) {
  return (
    <div className="bg-surface-container-highest rounded-xl p-12 text-center">
      <span className="material-symbols-outlined text-6xl text-on-surface-variant">{icon}</span>
      <h3 className="font-headline text-xl font-bold mt-3">{title}</h3>
      <p className="font-body text-on-surface-variant">{body}</p>
    </div>
  );
}

// ErrorState.tsx
"use client";
export function ErrorState({ reset }: { reset: () => void }) {
  return (
    <div className="bg-error-container text-on-error-container rounded-xl p-8 text-center">
      <h3 className="font-headline text-xl font-bold mb-2">حدث خطأ</h3>
      <button onClick={reset} className="px-4 py-2 rounded-full bg-error text-on-error font-bold">حاول مجدداً</button>
    </div>
  );
}
```

- [ ] **Step 2: `app/(dash)/loading.tsx`** — full-page skeleton shimmer with `bg-surface-container animate-pulse` placeholders for stat cards and lists.

- [ ] **Step 3: `app/(dash)/error.tsx`**

```tsx
"use client";
import { ErrorState } from "@/components/ErrorState";

export default function Error({ reset }: { error: Error; reset: () => void }) {
  return <ErrorState reset={reset} />;
}
```

- [ ] **Step 4: Replace inline empty messages with `EmptyState` in dashboard, library, chat-history, devices.**

- [ ] **Step 5: Commit** — `git commit -m "feat(frontend): unified loading/error/empty states"`

---

### Task 14: Bundle check + final smoke

- [ ] **Step 1: Build** — `docker compose run --rm frontend npm run build`. Should complete with no TypeScript errors.

- [ ] **Step 2: Lighthouse smoke** — open `http://localhost:3000` after login; check that fonts load (Cairo + Baloo Bhaijaan 2), `<html dir="rtl" lang="ar">` is present, no console errors.

- [ ] **Step 3: Cross-page click-through** — login → dashboard → library → semsem → back → chat-history → session → back → devices → add device → revoke device → logout. Each step should feel sub-200ms after warm-up.

- [ ] **Step 4: Final commit** — `git commit --allow-empty -m "chore(frontend): v0.1 ready"` (or skip if no diffs).

---

## Self-review

**Spec coverage check (PRD §4.2 web endpoints + §5 frontend directives):**

- Auth — Task 3 (login), Task 4 (logout in shell).
- Main dashboard aggregating `/api/users/me/stats` + `/api/devices` — Task 5.
- Library mapping `/api/semsems` with عادي / ذكي badges — Task 6 (with explicit unit test for both badges).
- Library detail mapping `/api/semsems/<uid>` — Task 7.
- Chat history list mapping `/api/chat-sessions` — Task 8.
- Chat detail with chat-bubble Child vs AI distinction — Task 9 (ChatBubble has unit test for both labels).
- Devices page with `DELETE /api/devices/<id>` revoke — Task 10.
- Add device flow generating provisioning token — Task 11.
- RTL + Arabic + Baloo Bhaijaan 2 + Cairo — Task 1 (`<html dir="rtl" lang="ar">`, fonts in tailwind config and globals.css).
- Loading/empty/error UX — Task 13.

**Placeholder scan:** none. Every task has runnable code, every test has a body, every command has expected output.

**Type/name consistency:** `Device.device_id`, `Semsem.uid_hex`, `is_pro`, `role` match backend serializers exactly. Stats response field names (`total_listening_ms`, `pro_total_ms`, `device_count`, `online_device_count`, `unique_semsems`) match the backend aggregator in backend Task 14.

**Open question for the implementer:** the Login page in `frontend/design.md` uses a hosted image as the logo. Replace with a self-hosted SVG before shipping (`public/nadeem-logo.svg`); the placeholder in Task 3 just renders text — fine for v0.1.

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-04-25-nadeem-frontend.md`. Two execution options:**

**1. Subagent-Driven (recommended)** — fresh subagent per task, review between tasks, fast iteration.
**2. Inline Execution** — execute tasks in this session using executing-plans, batch execution with checkpoints.

**Which approach?**
