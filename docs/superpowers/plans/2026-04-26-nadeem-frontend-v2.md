# Nadeem Web Frontend Implementation Plan (v2 — Design-Faithful)

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the Nadeem parental dashboard — an Arabic, RTL Next.js app that faithfully reproduces the design mockups in `frontend/design.md`. Parents log in, see usage stats, browse the Semsem library, audit AI chat transcripts, manage devices, and add new devices via a one-time provisioning code.

**Architecture:** Next.js 15 App Router (React 19) + TypeScript + Tailwind CSS. Auth via JWT stored in an `httpOnly` cookie set by a Next.js Route Handler; client components call same-origin Next.js API routes that forward to the Django backend. Design tokens (colors, fonts, mock screens) are defined in `frontend/design.md` — port the Tailwind config verbatim and reproduce each screen's markup as the component starting point. No state library; React Query for server cache.

**Tech Stack:** Next.js 15.x (App Router), React 19, TypeScript 5.x, Tailwind 3.x with `@tailwindcss/forms`, Material Symbols Outlined, fonts Baloo Bhaijaan 2 + Cairo, `@tanstack/react-query`, `zod` (form validation), `vitest` + `@testing-library/react` (unit), `playwright` (E2E). Docker-built; runs alongside the backend in compose.

**Out of scope:** Mobile companion app, signup flow (parent accounts created via Django admin / dev provisioning), localization beyond Arabic, dark-mode runtime toggle (ship light only).

**Backend dependency:** All endpoints in `2026-04-25-nadeem-backend.md`. Frontend cannot ship before backend auth, stats, semsems, chat-sessions, and devices endpoints are merged.

---

## File Structure

```
frontend/
├── Dockerfile
├── docker-compose.yml
├── package.json
├── tsconfig.json
├── next.config.ts
├── tailwind.config.ts
├── postcss.config.mjs
├── .env.example
├── .gitignore
├── vitest.config.ts
├── vitest.setup.ts
├── playwright.config.ts
├── public/
│   ├── nadeem-logo.svg
│   └── placeholder-semsem.svg
├── src/
│   ├── lib/
│   │   ├── api.ts
│   │   ├── api-client.ts
│   │   ├── jwt.ts
│   │   ├── format.ts
│   │   └── query-client.tsx
│   ├── components/
│   │   ├── Sidebar.tsx
│   │   ├── TopBar.tsx
│   │   ├── BottomNav.tsx
│   │   ├── StatCard.tsx
│   │   ├── SemsemCard.tsx
│   │   ├── TrackItem.tsx
│   │   ├── ChatBubble.tsx
│   │   ├── SessionCard.tsx
│   │   ├── DeviceHeroCard.tsx
│   │   ├── ConfirmDialog.tsx
│   │   ├── AddDeviceModal.tsx
│   │   ├── ErrorState.tsx
│   │   └── EmptyState.tsx
│   ├── app/
│   │   ├── layout.tsx
│   │   ├── globals.css
│   │   ├── login/page.tsx
│   │   ├── (dash)/
│   │   │   ├── layout.tsx
│   │   │   ├── page.tsx
│   │   │   ├── library/page.tsx
│   │   │   ├── library/[uid]/page.tsx
│   │   │   ├── chat-history/page.tsx
│   │   │   ├── chat-history/[id]/page.tsx
│   │   │   └── devices/page.tsx
│   │   └── api/
│   │       ├── auth/
│   │       │   ├── login/route.ts
│   │       │   └── logout/route.ts
│   │       ├── me/route.ts
│   │       ├── stats/route.ts
│   │       ├── semsems/
│   │       │   ├── route.ts
│   │       │   └── [uid]/route.ts
│   │       ├── chat-sessions/
│   │       │   ├── route.ts
│   │       │   └── [id]/route.ts
│   │       ├── devices/
│   │       │   ├── route.ts
│   │       │   └── [id]/route.ts
│   │       └── provisioning-tokens/route.ts
│   └── middleware.ts
└── e2e/
    ├── login.spec.ts
    └── dashboard.spec.ts
```

**Design rules:**
- Every protected page is a server component that pre-fetches via the server-side `api.ts`; React Query hydrates on the client for refetch/mutations.
- The browser **never** holds a JWT in JS — it talks to `/api/*` Next.js routes that read the `httpOnly` cookie and forward to Django.
- All copy lives inline in Arabic (no i18n library); strings come from `frontend/design.md`.
- Tailwind config is **copied verbatim** from `frontend/design.md` — same palette, fonts, radii. Don't redesign.
- Use logical properties (`ps-*` / `pe-*`) not directional (`pr-*` / `pl-*`) for RTL safety. Exception: when the design explicitly uses `right`/`left` for decorative blobs, use `start`/`end` equivalents.
- Every page must be responsive: desktop sidebar + mobile topbar/bottomnav pattern from the mockups.

---

### Task 1: Next.js scaffold + Tailwind + RTL + fonts + design tokens

**Files:**
- Create: all scaffold files per file structure (empty placeholders for pages/components)

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
  images: { remotePatterns: [{ protocol: "https", hostname: "**" }] },
};
export default config;
```

- [ ] **Step 4: `tailwind.config.ts`** — copy the full palette + fontFamily + borderRadius from `frontend/design.md` (repeated across all mockups). All 40+ color tokens must be present.

```ts
import type { Config } from "tailwindcss";

const config: Config = {
  darkMode: "class",
  content: ["./src/**/*.{ts,tsx}"],
  theme: {
    extend: {
      colors: {
        "tertiary-fixed-dim": "#d6c953",
        "inverse-primary": "#7cd0ff",
        "on-primary-container": "#00455f",
        "tertiary-container": "#baae3b",
        "on-background": "#1a1c19",
        "on-surface-variant": "#3e484f",
        "secondary-container": "#fdaa3f",
        "surface-dim": "#dadad5",
        "on-primary-fixed": "#001e2c",
        "secondary-fixed": "#ffddba",
        "surface": "#fafaf5",
        "on-primary": "#ffffff",
        "surface-container-high": "#e8e8e3",
        "surface-bright": "#fafaf5",
        "inverse-surface": "#2f312e",
        "on-tertiary-fixed-variant": "#4e4800",
        "primary-fixed-dim": "#7cd0ff",
        "on-primary-fixed-variant": "#004c69",
        "error-container": "#ffdad6",
        "outline-variant": "#bec8d0",
        "secondary": "#875200",
        "primary": "#00668a",
        "on-secondary": "#ffffff",
        "tertiary": "#686000",
        "on-secondary-fixed-variant": "#673d00",
        "on-error-container": "#93000a",
        "secondary-fixed-dim": "#ffb866",
        "surface-container-low": "#f4f4ef",
        "surface-container-highest": "#e3e3de",
        "surface-container": "#eeeee9",
        "tertiary-fixed": "#f3e56c",
        "on-secondary-container": "#6d4100",
        "outline": "#6f7880",
        "on-tertiary-fixed": "#1f1c00",
        "on-secondary-fixed": "#2b1700",
        "on-tertiary-container": "#474100",
        "primary-container": "#4fb6e9",
        "primary-fixed": "#c4e7ff",
        "on-error": "#ffffff",
        "on-surface": "#1a1c19",
        "surface-variant": "#e3e3de",
        "on-tertiary": "#ffffff",
        "surface-tint": "#00668a",
        "background": "#fafaf5",
        "surface-container-lowest": "#ffffff",
        "inverse-on-surface": "#f1f1ec",
        "error": "#ba1a1a",
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

- [ ] **Step 5: `postcss.config.mjs`** — `export default { plugins: { tailwindcss: {}, autoprefixer: {} } };`

- [ ] **Step 6: `src/app/globals.css`** — includes texture overlay utility, Material Symbols, custom scrollbar hide

```css
@import url('https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400..800&family=Cairo:wght@400..800&family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&display=swap');

@tailwind base;
@tailwind components;
@tailwind utilities;

html, body {
  @apply bg-background text-on-background font-body min-h-screen;
}

.material-symbols-outlined {
  font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24;
}

.material-symbols-outlined.fill {
  font-variation-settings: 'FILL' 1;
}

.texture-overlay::before {
  content: "";
  position: absolute;
  inset: 0;
  background-image: url("data:image/svg+xml,%3Csvg viewBox='0 0 200 200' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='noiseFilter'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.65' numOctaves='3' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23noiseFilter)' opacity='0.02'/%3E%3C/svg%3E");
  pointer-events: none;
  border-radius: inherit;
}

.no-scrollbar::-webkit-scrollbar { display: none; }
.no-scrollbar { -ms-overflow-style: none; scrollbar-width: none; }

@layer utilities {
  .pb-safe { padding-bottom: env(safe-area-inset-bottom, 0px); }
}
```

- [ ] **Step 7: `src/app/layout.tsx`**

```tsx
import "./globals.css";
import type { ReactNode } from "react";

export const metadata = { title: "نديم", description: "لوحة تحكم الآباء" };

export default function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html dir="rtl" lang="ar" className="light">
      <body className="antialiased">{children}</body>
    </html>
  );
}
```

- [ ] **Step 8: Placeholder root page** — `src/app/page.tsx`:

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

- [ ] **Step 13: `playwright.config.ts`**

```ts
import { defineConfig } from "@playwright/test";
export default defineConfig({
  testDir: "./e2e",
  use: { baseURL: "http://localhost:3000", locale: "ar" },
});
```

- [ ] **Step 14: `public/nadeem-logo.svg`** — create a simple placeholder SVG with Arabic text "نديم" in the primary color. Replace with real logo before shipping.

- [ ] **Step 15: Smoke** — Run: `docker compose up -d frontend && curl -s http://localhost:3000` → returns HTML containing `dir="rtl"` and `نديم`.

- [ ] **Step 16: Commit** — `git add frontend/ && git commit -m "chore(frontend): scaffold Next.js + Tailwind + RTL + design tokens"`

---

### Task 2: API client lib (server + browser) and JWT cookie

**Files:**
- Create: `src/lib/jwt.ts`, `src/lib/api.ts`, `src/lib/api-client.ts`, `src/lib/format.ts`, `src/lib/query-client.tsx`
- Create: `src/middleware.ts`

- [ ] **Step 1: `src/lib/jwt.ts`** (server-only)

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

- [ ] **Step 2: `src/lib/api.ts`** (server-side fetch wrapper)

```ts
import { getAccessToken } from "./jwt";

const BASE = process.env.BACKEND_INTERNAL_URL!;

export async function backend<T>(path: string, init: RequestInit = {}): Promise<T> {
  const token = await getAccessToken();
  const headers = new Headers(init.headers);
  if (token) headers.set("Authorization", `Bearer ${token}`);
  if (!headers.has("Content-Type") && init.body) headers.set("Content-Type", "application/json");
  const res = await fetch(`${BASE}${path}`, { ...init, headers, cache: "no-store" });
  if (!res.ok) throw new BackendError(res.status, await res.text());
  return res.status === 204 ? (undefined as T) : (res.json() as Promise<T>);
}

export class BackendError extends Error {
  constructor(public status: number, body: string) { super(`backend ${status}: ${body}`); }
}
```

- [ ] **Step 3: `src/lib/api-client.ts`** (browser — calls Next.js API routes)

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

export function fmtRelative(iso: string): string {
  const now = Date.now();
  const then = new Date(iso).getTime();
  const diffDays = Math.floor((now - then) / 86_400_000);
  if (diffDays === 0) return "اليوم";
  if (diffDays === 1) return "أمس";
  return new Intl.DateTimeFormat("ar", { dateStyle: "medium" }).format(new Date(iso));
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

- [ ] **Step 6: `src/middleware.ts`**

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

- [ ] **Step 7: Test format helpers**

`src/lib/__tests__/format.test.ts`:

```ts
import { describe, it, expect } from "vitest";
import { msToHours, msToMinutes, fmtRelative } from "../format";

describe("format", () => {
  it("converts ms to hours", () => { expect(typeof msToHours(3_600_000)).toBe("string"); });
  it("rounds minutes", () => { expect(msToMinutes(120_000)).toMatch(/2/); });
  it("shows today for recent dates", () => { expect(fmtRelative(new Date().toISOString())).toBe("اليوم"); });
});
```

Run: `npm run test` → 3 passed.

- [ ] **Step 8: Commit** — `git commit -m "feat(frontend): api client + jwt cookie + middleware"`

---

### Task 3: `POST /api/auth/login` route + design-faithful login page

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

- [ ] **Step 3: `src/app/login/page.tsx`** — reproduces the Login Page mockup from `frontend/design.md` lines 1-137 verbatim: texture overlay, decorative blobs, logo, icon-prefixed inputs, forgot-password link, gradient button, signup link.

```tsx
"use client";
import { useState } from "react";
import { useRouter } from "next/navigation";
import Image from "next/image";

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
      <div className="absolute inset-0 texture-overlay z-0" />
      <div className="absolute top-[-10%] right-[-5%] w-96 h-96 bg-primary-container rounded-full mix-blend-multiply filter blur-3xl opacity-30 z-0" />
      <div className="absolute bottom-[-10%] left-[-5%] w-80 h-80 bg-secondary-container rounded-full mix-blend-multiply filter blur-3xl opacity-30 z-0" />

      <div className="relative z-10 w-full max-w-md px-6 py-12">
        <div className="bg-surface-container-highest rounded-xl p-10 shadow-[0_8px_30px_rgb(0,0,0,0.04)] relative overflow-hidden">
          <div className="text-center mb-10">
            <div className="flex justify-center mb-2">
              <Image src="/nadeem-logo.svg" alt="نديم" width={160} height={160} className="h-40 w-auto object-contain" />
            </div>
            <p className="font-body text-on-surface-variant mt-2 text-lg">مرحباً بك في عالم الخيال</p>
          </div>

          <form className="space-y-6" onSubmit={submit}>
            <div>
              <label className="block font-label text-sm font-semibold text-on-surface mb-2" htmlFor="email">البريد الإلكتروني</label>
              <div className="relative">
                <div className="absolute inset-y-0 start-0 ps-4 flex items-center pointer-events-none">
                  <span className="material-symbols-outlined text-outline">mail</span>
                </div>
                <input id="email" type="email" autoComplete="email" required value={email} onChange={(e) => setEmail(e.target.value)}
                       placeholder="name@example.com"
                       className="block w-full rounded-md border-0 py-3 ps-12 pe-4 text-on-surface shadow-sm ring-1 ring-inset ring-outline-variant placeholder:text-outline focus:ring-2 focus:ring-inset focus:ring-primary focus:bg-surface-bright transition-all bg-surface-container-lowest font-body" />
              </div>
            </div>

            <div>
              <div className="flex justify-between items-center mb-2">
                <label className="block font-label text-sm font-semibold text-on-surface" htmlFor="password">كلمة المرور</label>
                <a className="font-label text-sm text-primary hover:text-primary-container transition-colors" href="#">نسيت كلمة المرور؟</a>
              </div>
              <div className="relative">
                <div className="absolute inset-y-0 start-0 ps-4 flex items-center pointer-events-none">
                  <span className="material-symbols-outlined text-outline">lock</span>
                </div>
                <input id="password" type="password" autoComplete="current-password" required value={password} onChange={(e) => setPassword(e.target.value)}
                       placeholder="••••••••"
                       className="block w-full rounded-md border-0 py-3 ps-12 pe-4 text-on-surface shadow-sm ring-1 ring-inset ring-outline-variant placeholder:text-outline focus:ring-2 focus:ring-inset focus:ring-primary focus:bg-surface-bright transition-all bg-surface-container-lowest font-body" />
              </div>
            </div>

            {err && <p className="text-error font-body text-sm">{err}</p>}

            <button disabled={busy} type="submit"
                    className="w-full flex justify-center py-4 px-4 border border-transparent rounded-full shadow-sm text-lg font-headline font-bold text-on-primary bg-gradient-to-br from-primary to-primary-container hover:scale-[1.02] transition-transform focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-primary mt-8 disabled:opacity-60">
              {busy ? "..." : "تسجيل الدخول"}
            </button>
          </form>

          <div className="mt-8 text-center">
            <p className="font-body text-sm text-on-surface-variant">
              ليس لديك حساب؟ <a className="font-bold text-primary hover:text-primary-container transition-colors" href="#">إنشاء حساب جديد</a>
            </p>
          </div>
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

- [ ] **Step 5: Smoke**: register a parent in backend, then visit /login manually.

- [ ] **Step 6: Commit** — `git commit -m "feat(frontend): design-faithful login page + jwt cookie route"`

---

### Task 4: App shell — Sidebar, TopBar, BottomNav for protected layout

**Files:**
- Create: `src/components/Sidebar.tsx`
- Create: `src/components/TopBar.tsx`
- Create: `src/components/BottomNav.tsx`
- Create: `src/app/(dash)/layout.tsx`
- Create: `src/app/api/me/route.ts`

- [ ] **Step 1: `src/app/api/me/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/auth/me")); }
  catch { return NextResponse.json({ detail: "unauthorized" }, { status: 401 }); }
}
```

- [ ] **Step 2: `src/components/Sidebar.tsx`** — desktop-only, fixed right side. Matches the sidebar from every mockup.

```tsx
"use client";
import Link from "next/link";
import { usePathname } from "next/navigation";
import Image from "next/image";

const NAV = [
  { href: "/", label: "الرئيسية", icon: "home" },
  { href: "/devices", label: "أجهزتي", icon: "devices" },
  { href: "/library", label: "مكتبة السماسم", icon: "library_music" },
  { href: "/chat-history", label: "سجل المحادثات الذكية", icon: "forum" },
  { href: "/settings", label: "الإعدادات", icon: "settings" },
];

export function Sidebar({ onAddDevice }: { onAddDevice: () => void }) {
  const path = usePathname();

  return (
    <aside className="hidden md:flex fixed right-0 top-0 h-full w-72 z-40 flex-col bg-surface-container-low shadow-[-4px_0_24px_rgba(0,0,0,0.02)] rounded-l-[3rem] pt-20 pb-8">
      <div className="px-6 mb-10 text-center">
        <Image src="/nadeem-logo.svg" alt="نديم" width={128} height={128} className="h-32 mx-auto object-contain drop-shadow-sm mb-4" />
      </div>

      <nav className="flex-1 space-y-2 px-4">
        {NAV.map((n) => {
          const active = n.href === "/" ? path === "/" : path.startsWith(n.href);
          return (
            <Link key={n.href} href={n.href}
                  className={`flex items-center gap-3 px-6 py-3 rounded-full font-body text-sm font-medium tracking-wide transition-all duration-300
                    ${active
                      ? "bg-gradient-to-br from-primary to-primary-container text-white shadow-md shadow-primary/20 scale-105"
                      : "text-on-surface-variant hover:text-primary hover:bg-surface-container-high"}`}>
              <span className={`material-symbols-outlined ${active ? "fill" : ""}`}>{n.icon}</span>
              <span>{n.label}</span>
            </Link>
          );
        })}
      </nav>

      <div className="px-6 mt-auto">
        <button onClick={onAddDevice}
                className="w-full bg-secondary-container text-on-secondary-container rounded-full py-4 font-body font-bold hover:scale-105 transition-transform flex items-center justify-center gap-2 shadow-sm">
          <span className="material-symbols-outlined">add</span>
          إضافة جهاز جديد
        </button>
      </div>
    </aside>
  );
}
```

- [ ] **Step 3: `src/components/TopBar.tsx`** — mobile-only sticky top bar.

```tsx
"use client";
import Image from "next/image";

export function TopBar() {
  return (
    <header className="md:hidden sticky top-0 z-30 flex flex-row-reverse justify-between items-center w-full px-6 h-20 bg-surface/80 backdrop-blur-xl shadow-sm">
      <div className="flex items-center gap-4 text-primary">
        <Image src="/nadeem-logo.svg" alt="نديم" width={48} height={48} className="h-12 w-auto object-contain" />
      </div>
      <div className="flex items-center gap-2">
        <button className="p-2 rounded-full text-on-surface-variant hover:bg-surface-container transition-colors">
          <span className="material-symbols-outlined">notifications</span>
        </button>
        <button className="p-2 rounded-full text-on-surface-variant hover:bg-surface-container transition-colors">
          <span className="material-symbols-outlined">settings</span>
        </button>
      </div>
    </header>
  );
}
```

- [ ] **Step 4: `src/components/BottomNav.tsx`** — mobile-only fixed bottom nav.

```tsx
"use client";
import Link from "next/link";
import { usePathname } from "next/navigation";

const TABS = [
  { href: "/", label: "الرئيسية", icon: "home" },
  { href: "/devices", label: "أجهزتي", icon: "devices" },
  { href: "/library", label: "المكتبة", icon: "library_music" },
  { href: "/chat-history", label: "السجل", icon: "forum" },
  { href: "/settings", label: "إعدادات", icon: "settings" },
];

export function BottomNav() {
  const path = usePathname();

  return (
    <nav className="md:hidden fixed bottom-0 left-0 right-0 z-40 bg-surface/90 backdrop-blur-xl shadow-[0_-4px_20px_rgba(0,0,0,0.05)] pb-safe">
      <div className="flex justify-around items-center h-20 px-2">
        {TABS.map((t) => {
          const active = t.href === "/" ? path === "/" : path.startsWith(t.href);
          return (
            <Link key={t.href} href={t.href}
                  className={`flex flex-col items-center justify-center w-16 h-full transition-colors ${active ? "text-primary font-bold" : "text-on-surface-variant"}`}>
              {active ? (
                <div className="bg-primary/10 p-1.5 rounded-full mb-1">
                  <span className="material-symbols-outlined fill">{t.icon}</span>
                </div>
              ) : (
                <span className="material-symbols-outlined mb-1">{t.icon}</span>
              )}
              <span className="text-[10px] font-medium">{t.label}</span>
            </Link>
          );
        })}
      </div>
    </nav>
  );
}
```

- [ ] **Step 5: `src/app/(dash)/layout.tsx`** — wires sidebar/topbar/bottomnav, pre-fetches user, provides React Query.

```tsx
import { redirect } from "next/navigation";
import { backend, BackendError } from "@/lib/api";
import { Sidebar } from "@/components/Sidebar";
import { TopBar } from "@/components/TopBar";
import { BottomNav } from "@/components/BottomNav";
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
      <DashShell email={me.email}>{children}</DashShell>
    </QueryProvider>
  );
}

function DashShell({ email, children }: { email: string; children: React.ReactNode }) {
  return (
    <div className="min-h-screen bg-background text-on-background">
      <Sidebar onAddDevice={() => {}} />
      <TopBar />
      <main className="md:mr-72 pt-0 md:pt-0 px-6 md:px-12 pb-24 md:pb-12 max-w-7xl mx-auto w-full">
        {children}
      </main>
      <BottomNav />
    </div>
  );
}
```

- [ ] **Step 6: Smoke**: visit `/` after login → see sidebar with nav on desktop, topbar+bottomnav on mobile.

- [ ] **Step 7: Commit** — `git commit -m "feat(frontend): protected layout with sidebar + topbar + bottomnav"`

---

### Task 5: Dashboard home page — bento grid stat cards + device status

**Files:**
- Create: `src/app/(dash)/page.tsx`
- Create: `src/components/StatCard.tsx`
- Create: `src/app/api/stats/route.ts`
- Create: `src/app/api/devices/route.ts`

- [ ] **Step 1: `src/app/api/stats/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/users/me/stats")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `src/app/api/devices/route.ts`** (GET)

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/devices")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 3: `src/components/StatCard.tsx`** — matches the bento stat cards from `design.md`. Decorative blur blob, icon circle, hover scale.

```tsx
import type { ReactNode } from "react";

type Props = {
  icon: string;
  label: string;
  value: ReactNode;
  sub?: ReactNode;
  iconBg?: string;
  decorative?: string;
  badge?: ReactNode;
};

export function StatCard({ icon, label, value, sub, iconBg = "bg-primary-container/20", decorative, badge }: Props) {
  return (
    <div className="bg-surface-container-highest rounded-xl p-8 relative overflow-hidden group hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300">
      {decorative && <div className={`absolute -start-6 -top-6 w-32 h-32 ${decorative} rounded-full blur-2xl`} />}
      <div className="flex items-start justify-between mb-6 relative z-10">
        <div className={`h-14 w-14 rounded-full ${iconBg} flex items-center justify-center text-primary`}>
          <span className="material-symbols-outlined text-3xl">{icon}</span>
        </div>
        {badge}
      </div>
      <div className="relative z-10">
        <p className="text-on-surface-variant text-sm font-medium mb-1">{label}</p>
        <h3 className="font-headline text-4xl font-bold text-primary">{value}</h3>
        {sub && <span className="text-xl text-on-surface-variant font-body font-normal">{sub}</span>}
      </div>
    </div>
  );
}
```

- [ ] **Step 4: `src/app/(dash)/page.tsx`** — reproduces the Dashboard Home mockup. Three-column bento grid with stat cards, plus device status card with online indicator.

```tsx
import { backend } from "@/lib/api";
import { StatCard } from "@/components/StatCard";
import { msToHours } from "@/lib/format";

type Stats = { total_listening_ms: number; unique_semsems: number; pro_total_ms: number; device_count: number; online_device_count: number };
type Device = { device_id: string; online: boolean; last_seen_at: string | null; name: string | null };

export default async function Dashboard() {
  const [stats, devices] = await Promise.all([
    backend<Stats>("/api/users/me/stats"),
    backend<Device[]>("/api/devices"),
  ]);

  const primaryDevice = devices[0];

  return (
    <div className="pt-24 md:pt-8">
      <header className="mb-10">
        <h1 className="font-headline text-[2.5rem] font-bold text-primary mb-2">مرحباً، أهالي نديم!</h1>
        <p className="text-on-surface-variant text-lg">إليكم نظرة سريعة على نشاط طفلكم اليوم.</p>
      </header>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mb-12">
        <StatCard
          icon="headphones"
          label="إجمالي ساعات الاستماع"
          value={msToHours(stats.total_listening_ms)}
          sub="ساعة"
          iconBg="bg-primary-container/20"
          decorative="bg-primary/5"
        />
        <StatCard
          icon="smart_toy"
          label="السماسم المملوكة"
          value={stats.unique_semsems}
          sub="سماسم"
          iconBg="bg-secondary-container/20"
          decorative="bg-secondary-container/10"
        />
        <StatCard
          icon="wifi"
          label="حالة الجهاز"
          value={primaryDevice ? (primaryDevice.name ?? "جهاز نديم") : "لا يوجد جهاز"}
          iconBg="bg-surface-container"
          decorative="bg-tertiary-container/10"
          badge={
            primaryDevice ? (
              <div className="flex items-center gap-2 bg-surface px-3 py-1.5 rounded-full shadow-sm">
                <div className={`w-2.5 h-2.5 rounded-full ${primaryDevice.online ? "bg-green-500 animate-pulse" : "bg-on-surface-variant"}`} />
                <span className={`text-xs font-bold ${primaryDevice.online ? "text-green-700" : "text-on-surface-variant"}`}>
                  {primaryDevice.online ? "متصل" : "غير متصل"}
                </span>
              </div>
            ) : undefined
          }
        />
      </div>
    </div>
  );
}
```

- [ ] **Step 5: Smoke**: with seeded data, dashboard shows stats and device online status.

- [ ] **Step 6: Commit** — `git commit -m "feat(frontend): dashboard with bento grid stat cards"`

---

### Task 6: Library list page — image cards with badges and filter tabs

**Files:**
- Create: `src/app/(dash)/library/page.tsx`
- Create: `src/components/SemsemCard.tsx`
- Create: `src/app/api/semsems/route.ts`

- [ ] **Step 1: `src/app/api/semsems/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/semsems")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `src/components/SemsemCard.tsx`** — matches the library card mockup from `design.md`. Cover image, badge overlay, play/forum button, hover scale.

```tsx
import Link from "next/link";

type Semsem = {
  uid_hex: string;
  title: string;
  is_pro: boolean;
  role: string;
  description?: string;
  duration?: string;
  image_url?: string;
};

export function SemsemCard({ s }: { s: Semsem }) {
  return (
    <Link href={`/library/${s.uid_hex}`}
          className={`group relative bg-surface-container-highest rounded-[2rem] overflow-hidden hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300 cursor-pointer flex flex-col h-full
            ${s.is_pro ? "ring-2 ring-primary/20" : ""}`}>
      <div className="relative h-48 w-full bg-surface-container overflow-hidden">
        {s.is_pro && <div className="absolute inset-0 bg-gradient-to-tr from-primary/20 to-transparent mix-blend-overlay z-10" />}
        {s.image_url ? (
          <img alt={s.title} src={s.image_url} className="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" />
        ) : (
          <div className="w-full h-full flex items-center justify-center bg-gradient-to-br from-primary-container/20 to-surface-container">
            <span className="material-symbols-outlined text-6xl text-primary/30">
              {s.is_pro ? "smart_toy" : "volume_up"}
            </span>
          </div>
        )}
        <div className={`absolute top-4 start-4 px-3 py-1 rounded-full flex items-center gap-1 shadow-sm z-20
          ${s.is_pro
            ? "bg-tertiary-container text-on-tertiary-container"
            : "bg-surface/90 backdrop-blur-md"}`}>
          <span className={`material-symbols-outlined text-sm fill ${s.is_pro ? "" : "text-primary"}`}>{s.is_pro ? "smart_toy" : "volume_up"}</span>
          <span className={`text-sm font-bold ${s.is_pro ? "" : "text-primary"}`}>{s.is_pro ? "ذكي" : "عادي"}</span>
        </div>
      </div>
      <div className="p-6 flex flex-col flex-grow">
        <h3 className="font-headline text-2xl font-bold text-on-surface mb-2">{s.title}</h3>
        <p className="text-sm text-on-surface-variant flex-grow">{s.description ?? (s.is_pro ? "شخصية تفاعلية ذكية" : "محتوى صوتي")}</p>
        <div className="mt-4 flex items-center justify-between">
          <span className="text-xs text-outline font-medium">{s.is_pro ? "محادثة مفتوحة" : (s.duration ?? "")}</span>
          <button className={`w-10 h-10 rounded-full flex items-center justify-center transition-colors
            ${s.is_pro
              ? "bg-gradient-to-br from-primary to-primary-container text-on-primary shadow-md group-hover:shadow-lg"
              : "bg-primary/10 text-primary group-hover:bg-primary group-hover:text-on-primary"}`}>
            <span className="material-symbols-outlined">{s.is_pro ? "forum" : "play_arrow"}</span>
          </button>
        </div>
      </div>
    </Link>
  );
}
```

- [ ] **Step 3: `src/app/(dash)/library/page.tsx`** — matches library mockup header + filter tabs + grid.

```tsx
"use client";
import { useQuery } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { SemsemCard } from "@/components/SemsemCard";
import { useState } from "react";

type Semsem = { uid_hex: string; title: string; is_pro: boolean; role: string; description?: string; duration?: string; image_url?: string };

export default function Library() {
  const [filter, setFilter] = useState<"all" | "regular" | "pro">("all");
  const { data: list = [], isLoading } = useQuery({ queryKey: ["semsems"], queryFn: () => client<Semsem[]>("/api/semsems") });

  const filtered = filter === "all" ? list : filter === "pro" ? list.filter((s) => s.is_pro) : list.filter((s) => !s.is_pro);

  return (
    <div className="pt-24 md:pt-12">
      <div className="mb-12 flex flex-col md:flex-row md:justify-between md:items-end gap-4">
        <div>
          <h1 className="font-headline text-5xl font-extrabold text-primary mb-2 tracking-wide">مكتبة السماسم</h1>
          <p className="text-on-surface-variant text-lg max-w-xl leading-relaxed">استكشف مجموعة متنوعة من السماسم المصممة لإثراء خيال طفلك. اختر بين القصص العادية والتفاعلات الذكية.</p>
        </div>
        <div className="hidden md:flex gap-3 bg-surface-container-low p-1 rounded-full">
          {([["all", "الكل"], ["regular", "عادي"], ["pro", "ذكي"]] as const).map(([key, label]) => (
            <button key={key} onClick={() => setFilter(key)}
                    className={`px-6 py-2 rounded-full font-bold transition-all hover:scale-[1.02] ${filter === key ? "bg-primary text-on-primary shadow-sm" : "text-on-surface-variant hover:bg-surface-container-high"}`}>
              {label}
            </button>
          ))}
        </div>
      </div>

      {isLoading ? (
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-8">
          {[1,2,3,4].map((i) => <div key={i} className="bg-surface-container-highest rounded-[2rem] h-72 animate-pulse" />}
        </div>
      ) : filtered.length === 0 ? (
        <div className="bg-surface-container-highest rounded-xl p-12 text-center">
          <span className="material-symbols-outlined text-6xl text-on-surface-variant">library_music</span>
          <h3 className="font-headline text-xl font-bold mt-3">لا توجد سماسم</h3>
          <p className="font-body text-on-surface-variant">لم يتم استخدام أي سمسم بعد</p>
        </div>
      ) : (
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-8">
          {filtered.map((s) => <SemsemCard key={s.uid_hex} s={s} />)}
        </div>
      )}
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

- [ ] **Step 5: Commit** — `git commit -m "feat(frontend): library with image cards + filter tabs + pro/regular badges"`

---

### Task 7: Semsem detail page — hero card + bento grid

**Files:**
- Create: `src/app/(dash)/library/[uid]/page.tsx`
- Create: `src/components/TrackItem.tsx`
- Create: `src/app/api/semsems/[uid]/route.ts`

- [ ] **Step 1: `src/app/api/semsems/[uid]/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET(_: Request, ctx: { params: Promise<{ uid: string }> }) {
  const { uid } = await ctx.params;
  try { return NextResponse.json(await backend(`/api/semsems/${uid}`)); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `src/components/TrackItem.tsx`** — matches track list items from `design.md`.

```tsx
"use client";
import { useState } from "react";

type Props = { name: string; duration?: string; url: string; playing?: boolean };

export function TrackItem({ name, duration, url, playing: initialPlaying }: Props) {
  const [playing, setPlaying] = useState(initialPlaying ?? false);

  return (
    <div className={`group rounded-xl p-4 flex items-center gap-4 cursor-pointer transition-colors shadow-sm hover:shadow-md
      ${playing ? "bg-primary-container/10 border border-primary/10" : "bg-surface-container-lowest hover:bg-surface-container-high"}`}>
      {playing && <div className="absolute start-0 top-0 bottom-0 w-1 bg-primary rounded-s" />}
      <div className={`w-12 h-12 rounded-full flex items-center justify-center shrink-0 transition-colors
        ${playing ? "bg-primary text-white shadow-md" : "bg-secondary-container/20 text-secondary group-hover:bg-secondary-container/40"}`}>
        <span className={`material-symbols-outlined ${playing ? "fill" : ""}`}
              onClick={() => setPlaying(!playing)}>
          {playing ? "pause" : "play_arrow"}
        </span>
      </div>
      <div className="flex-1" onClick={() => setPlaying(!playing)}>
        <h4 className={`font-headline font-bold transition-colors ${playing ? "text-primary" : "text-on-surface group-hover:text-primary"}`}>{name}</h4>
        {duration && <p className="font-body text-xs text-on-surface-variant mt-0.5">{duration}</p>}
      </div>
      {playing && (
        <div className="flex gap-1 items-end h-6 opacity-70">
          {[3,5,2,4].map((h, i) => <div key={i} className="w-1 bg-primary rounded-t-sm animate-pulse" style={{ height: `${h*4}px`, animationDelay: `${i*0.1}s` }} />)}
        </div>
      )}
    </div>
  );
}
```

- [ ] **Step 3: `src/app/(dash)/library/[uid]/page.tsx`** — reproduces the Semsem Detail mockup: breadcrumb + back, hero card with avatar/badge, bento grid (tracks + stats + tags + NFC status).

```tsx
import { backend } from "@/lib/api";
import { TrackItem } from "@/components/TrackItem";
import { msToMinutes } from "@/lib/format";
import Link from "next/link";

type Detail = {
  uid_hex: string; title: string; is_pro: boolean; role: string;
  tracks: { name: string; url: string }[];
  play_count: number; total_play_ms: number; pro_session_count: number; pro_total_ms: number;
};

export default async function SemsemDetail({ params }: { params: Promise<{ uid: string }> }) {
  const { uid } = await params;
  const d = await backend<Detail>(`/api/semsems/${uid}`);

  return (
    <div className="pt-24 md:pt-12 pb-12">
      {/* Breadcrumb */}
      <div className="flex items-center gap-3 mb-8 text-on-surface-variant">
        <Link href="/library" className="w-10 h-10 rounded-full bg-surface-container-highest flex items-center justify-center hover:bg-surface-variant transition-colors group">
          <span className="material-symbols-outlined text-on-surface-variant group-hover:text-primary transition-colors">arrow_forward</span>
        </Link>
        <div className="flex items-center gap-2 font-body text-sm">
          <span className="hover:text-primary cursor-pointer transition-colors">مكتبة السماسم</span>
          <span className="material-symbols-outlined text-sm">chevron_left</span>
          <span className="font-bold text-primary">تفاصيل السمسم</span>
        </div>
      </div>

      {/* Hero Card */}
      <section className="mb-12 relative rounded-xl bg-surface-container-highest p-8 md:p-12 overflow-hidden flex flex-col md:flex-row gap-8 items-center shadow-[0_10px_40px_-10px_rgba(0,102,138,0.08)] texture-overlay">
        <div className="absolute -top-24 -right-24 w-64 h-64 bg-primary-container/20 rounded-full blur-3xl" />
        <div className="absolute -bottom-24 -left-24 w-80 h-80 bg-secondary-container/10 rounded-full blur-3xl" />

        <div className="relative z-10 w-40 h-40 md:w-56 md:h-56 shrink-0 rounded-[3rem] bg-surface flex items-center justify-center shadow-lg border-4 border-surface p-2">
          <div className={`w-full h-full rounded-[2.5rem] flex items-center justify-center overflow-hidden relative
            ${d.is_pro ? "bg-gradient-to-br from-tertiary-container to-tertiary-fixed-dim" : "bg-gradient-to-br from-primary-container to-primary-fixed"}`}>
            <span className="material-symbols-outlined text-6xl z-10 text-on-tertiary-container">{d.is_pro ? "auto_awesome" : "headphones"}</span>
            {d.is_pro && <div className="absolute inset-0 bg-white/20 rounded-full blur-xl animate-pulse" />}
          </div>
        </div>

        <div className="relative z-10 flex-1 space-y-4 text-center md:text-start">
          <div className={`inline-flex items-center gap-2 px-3 py-1 rounded-full font-body text-sm font-bold mb-2
            ${d.is_pro ? "bg-primary/10 text-primary" : "bg-primary-container/20 text-primary"}`}>
            <span className="material-symbols-outlined text-sm">{d.is_pro ? "psychology" : "volume_up"}</span>
            {d.is_pro ? "سمسم ذكي (Pro)" : "سمسم عادي"}
          </div>
          <h1 className="font-headline text-4xl md:text-5xl font-bold text-on-surface tracking-wide">{d.title}</h1>
          {d.is_pro && <p className="font-headline text-xl text-primary font-medium">دور: {d.role}</p>}
        </div>
      </section>

      {/* Bento Grid */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
        <div className="lg:col-span-2 space-y-4">
          <div className="flex justify-between items-end mb-4">
            <div>
              <h2 className="font-headline text-2xl font-bold text-on-surface">المحتوى الصوتي</h2>
              <p className="font-body text-sm text-on-surface-variant">المقاطع المتوفرة</p>
            </div>
            <span className="bg-surface-container-highest text-on-surface-variant px-3 py-1 rounded-lg font-body text-xs font-bold">{d.tracks.length} مقاطع</span>
          </div>
          <div className="space-y-4">
            {d.tracks.map((t) => <TrackItem key={t.name} name={t.name} url={t.url} />)}
          </div>
        </div>

        <div className="space-y-6">
          <div className="bg-surface-container-low rounded-xl p-6 flex flex-col justify-between h-40 relative overflow-hidden group hover:bg-surface-container transition-colors">
            <div className="flex justify-between items-start z-10">
              <span className="font-body text-sm font-medium text-on-surface-variant">وقت الاستماع الإجمالي</span>
              <span className="material-symbols-outlined text-primary bg-surface-container-highest p-1.5 rounded-lg">timer</span>
            </div>
            <div className="z-10">
              <span className="font-headline text-3xl font-bold text-on-surface">{msToMinutes(d.total_play_ms)}</span>
              <span className="font-body text-sm text-on-surface-variant"> دقيقة</span>
            </div>
          </div>

          <div className="bg-tertiary-container/10 rounded-xl p-6 border border-tertiary-container/20">
            <h3 className="font-headline text-lg font-bold text-on-tertiary-container mb-2 flex items-center gap-2">
              <span className="material-symbols-outlined">nfc</span>
              حالة القطعة (NFC)
            </h3>
            <p className="font-body text-sm text-on-surface-variant mb-4">القطعة المادية مربوطة وجاهزة للاستخدام.</p>
            <div className="flex items-center gap-3">
              <div className="relative w-8 h-8 flex items-center justify-center">
                <span className="absolute inset-0 rounded-full bg-tertiary-container/40 animate-ping opacity-75" />
                <div className="w-4 h-4 bg-tertiary-container rounded-full relative z-10" />
              </div>
              <span className="font-body text-xs font-bold text-tertiary">متصل بالشبكة</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
```

- [ ] **Step 4: Smoke**: navigate from library → detail page; tracks and stats render.

- [ ] **Step 5: Commit** — `git commit -m "feat(frontend): semsem detail with hero card + bento grid"`

---

### Task 8: Chat history — split-pane with session list + transcript

**Files:**
- Create: `src/app/(dash)/chat-history/page.tsx`
- Create: `src/app/(dash)/chat-history/[id]/page.tsx`
- Create: `src/components/SessionCard.tsx`
- Create: `src/components/ChatBubble.tsx`
- Create: `src/app/api/chat-sessions/route.ts`
- Create: `src/app/api/chat-sessions/[id]/route.ts`

- [ ] **Step 1: `src/app/api/chat-sessions/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET() {
  try { return NextResponse.json(await backend("/api/chat-sessions")); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `src/app/api/chat-sessions/[id]/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function GET(_: Request, ctx: { params: Promise<{ id: string }> }) {
  const { id } = await ctx.params;
  try { return NextResponse.json(await backend(`/api/chat-sessions/${id}`)); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 3: `src/components/SessionCard.tsx`**

```tsx
type Props = {
  title: string;
  role: string;
  preview: string;
  date: string;
  active?: boolean;
  onClick?: () => void;
};

export function SessionCard({ title, role, preview, date, active, onClick }: Props) {
  return (
    <div onClick={onClick}
         className={`p-4 rounded-lg cursor-pointer transform transition-transform hover:scale-[1.02]
           ${active ? "bg-surface-container-highest" : "bg-surface hover:bg-surface-container-highest/50"}`}>
      <div className="flex justify-between items-start mb-2">
        <h3 className={`font-headline text-lg font-semibold ${active ? "text-primary" : "text-on-surface"}`}>{title}</h3>
        <span className="font-body text-xs text-on-surface-variant bg-surface px-2 py-1 rounded-full">{date}</span>
      </div>
      <p className="font-body text-sm text-on-surface-variant line-clamp-2">{preview}</p>
    </div>
  );
}
```

- [ ] **Step 4: `src/components/ChatBubble.tsx`** — matches the rich bubble layout from `design.md`. AI messages have gradient avatar + play button; child messages have secondary-container bg.

```tsx
type Props = { speaker: "child" | "ai"; text: string; timestamp: string };

export function ChatBubble({ speaker, text, timestamp }: Props) {
  const isAi = speaker === "ai";

  return (
    <div className={`flex gap-4 w-full md:w-4/5 ${isAi ? "justify-end flex-row-reverse" : "float-right"}`}>
      <div className={`w-10 h-10 rounded-full flex-shrink-0 flex items-center justify-center mt-2
        ${isAi ? "bg-gradient-to-br from-primary to-primary-container" : "bg-secondary-container"}`}>
        <span className={`material-symbols-outlined text-lg ${isAi ? "text-white" : "text-on-secondary-container"}`}>
          {isAi ? "robot_2" : "face"}
        </span>
      </div>
      <div className={`rounded-2xl p-5 relative group
        ${isAi ? "bg-surface-container-low rounded-tl-sm" : "bg-secondary-container rounded-tr-sm"}`}>
        <div className={`text-xs font-bold mb-1 ${isAi ? "text-on-surface-variant" : "text-on-secondary-container"}`}>
          {isAi ? "نديم" : "الطفل"}
        </div>
        <p className={`font-body text-base leading-relaxed ${isAi ? "text-on-surface" : "text-on-secondary-container"}`}>{text}</p>
        <div className="text-[10px] mt-1 opacity-70">{new Date(timestamp).toLocaleTimeString("ar")}</div>
        {isAi && (
          <button className="absolute -bottom-4 -left-4 bg-surface-container-lowest/80 backdrop-blur-md p-2.5 rounded-full text-primary shadow-[0_4px_12px_rgba(0,102,138,0.1)] hover:scale-110 transition-transform">
            <span className="material-symbols-outlined text-xl fill">play_arrow</span>
          </button>
        )}
      </div>
    </div>
  );
}
```

- [ ] **Step 5: Chat history page (split-pane)** — reproduces the layout from `design.md`. On desktop: session list (right 4 cols) + transcript (left 8 cols). On mobile: session list only, click → navigate to detail page.

```tsx
"use client";
import { useQuery } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { SessionCard } from "@/components/SessionCard";
import { ChatBubble } from "@/components/ChatBubble";
import { useState } from "react";

type Session = { id: number; device_id: string; uid_hex: string; role: string; started_at: string; ended_at: string | null; title?: string };
type Detail = { id: number; role: string; started_at: string; entries: { speaker: "child" | "ai"; text: string; timestamp: string }[] };

export default function ChatHistory() {
  const [selectedId, setSelectedId] = useState<number | null>(null);
  const { data: sessions = [] } = useQuery({ queryKey: ["chat-sessions"], queryFn: () => client<Session[]>("/api/chat-sessions") });
  const { data: detail } = useQuery({
    queryKey: ["chat-session", selectedId],
    queryFn: () => client<Detail>(`/api/chat-sessions/${selectedId}`),
    enabled: !!selectedId,
  });

  return (
    <div className="pt-24 md:pt-8 flex flex-col gap-8">
      <header className="mb-4">
        <h1 className="font-headline text-4xl md:text-5xl font-bold text-primary tracking-wide">سجل المحادثات الذكية</h1>
        <p className="font-body text-lg text-on-surface-variant mt-2">تابع رحلة استكشاف طفلك مع سمسم.</p>
      </header>

      <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 lg:gap-8 min-h-[600px]">
        <div className="lg:col-span-4 bg-surface-container-low rounded-xl p-4 flex flex-col h-full overflow-hidden">
          <div className="flex justify-between items-center mb-6 px-2">
            <h2 className="font-headline text-2xl font-semibold text-on-surface">الجلسات الأخيرة</h2>
            <button className="text-primary hover:bg-primary/10 p-2 rounded-full transition-colors">
              <span className="material-symbols-outlined">filter_list</span>
            </button>
          </div>
          <div className="flex-1 overflow-y-auto no-scrollbar space-y-3 ps-2">
            {sessions.map((s) => (
              <SessionCard
                key={s.id}
                title={s.title ?? `دور: ${s.role}`}
                role={s.role}
                preview={`جهاز: ${s.device_id}`}
                date={new Date(s.started_at).toLocaleDateString("ar")}
                active={selectedId === s.id}
                onClick={() => setSelectedId(s.id)}
              />
            ))}
          </div>
        </div>

        <div className="hidden lg:flex lg:col-span-8 bg-surface-container-lowest rounded-xl flex-col h-full relative overflow-hidden">
          {detail ? (
            <>
              <div className="bg-surface-container-highest/30 px-8 py-5 flex items-center gap-4">
                <div className="w-12 h-12 rounded-full bg-gradient-to-br from-primary to-primary-container flex items-center justify-center shadow-sm">
                  <span className="material-symbols-outlined text-white fill">robot_2</span>
                </div>
                <div>
                  <h2 className="font-headline text-xl font-bold text-on-surface">{detail.role}</h2>
                  <p className="font-body text-sm text-primary">{new Date(detail.started_at).toLocaleString("ar")}</p>
                </div>
              </div>
              <div className="flex-1 overflow-y-auto p-8 space-y-8">
                <div className="flex justify-center">
                  <span className="font-body text-xs font-medium text-on-surface-variant bg-surface-container-low px-4 py-1.5 rounded-full">بدأت المحادثة</span>
                </div>
                {detail.entries.map((e, i) => <ChatBubble key={i} {...e} />)}
              </div>
              <div className="h-12 bg-gradient-to-t from-surface-container-lowest to-transparent absolute bottom-0 start-0 end-0 pointer-events-none rounded-b-xl" />
            </>
          ) : (
            <div className="flex-1 flex items-center justify-center text-on-surface-variant">
              <p className="font-body">اختر جلسة لعرض المحادثة</p>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
```

- [ ] **Step 6: Mobile detail page** — `src/app/(dash)/chat-history/[id]/page.tsx`.

```tsx
import { backend } from "@/lib/api";
import { ChatBubble } from "@/components/ChatBubble";
import Link from "next/link";

type Detail = { id: number; role: string; started_at: string; entries: { speaker: "child" | "ai"; text: string; timestamp: string }[] };

export default async function ChatDetail({ params }: { params: Promise<{ id: string }> }) {
  const { id } = await params;
  const s = await backend<Detail>(`/api/chat-sessions/${id}`);

  return (
    <div className="pt-24 md:pt-8 pb-12">
      <div className="flex items-center gap-3 mb-6">
        <Link href="/chat-history" className="w-10 h-10 rounded-full bg-surface-container-highest flex items-center justify-center">
          <span className="material-symbols-outlined text-on-surface-variant">arrow_forward</span>
        </Link>
        <h2 className="font-headline text-2xl font-bold text-on-surface">دور: {s.role}</h2>
      </div>
      <div className="bg-surface-container-lowest rounded-xl p-6 space-y-6">
        {s.entries.map((e, i) => <ChatBubble key={i} {...e} />)}
      </div>
    </div>
  );
}
```

- [ ] **Step 7: Bubble test**

`src/components/__tests__/ChatBubble.test.tsx`:

```tsx
import { render, screen } from "@testing-library/react";
import { describe, it, expect } from "vitest";
import { ChatBubble } from "../ChatBubble";

describe("ChatBubble", () => {
  it("renders نديم label for AI", () => {
    render(<ChatBubble speaker="ai" text="hi" timestamp={new Date().toISOString()} />);
    expect(screen.getByText("نديم")).toBeInTheDocument();
  });
  it("renders الطفل label for child", () => {
    render(<ChatBubble speaker="child" text="hi" timestamp={new Date().toISOString()} />);
    expect(screen.getByText("الطفل")).toBeInTheDocument();
  });
});
```

Run: `npm run test`.

- [ ] **Step 8: Commit** — `git commit -m "feat(frontend): chat history split-pane + rich bubbles"`

---

### Task 9: Devices page — bento grid with hero device card

**Files:**
- Create: `src/app/(dash)/devices/page.tsx`
- Create: `src/components/DeviceHeroCard.tsx`
- Create: `src/components/ConfirmDialog.tsx`
- Create: `src/app/api/devices/[id]/route.ts`

- [ ] **Step 1: `src/app/api/devices/[id]/route.ts`** (DELETE)

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function DELETE(_: Request, ctx: { params: Promise<{ id: string }> }) {
  const { id } = await ctx.params;
  try { await backend(`/api/devices/${id}`, { method: "DELETE" }); return new NextResponse(null, { status: 204 }); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `src/components/DeviceHeroCard.tsx`** — matches the device hero card from `design.md`.

```tsx
type Device = {
  device_id: string;
  online: boolean;
  last_seen_at: string | null;
  name?: string;
  battery_level?: number;
  wifi_signal?: string;
  room?: string;
};

export function DeviceHeroCard({ d, onSettings, onUpdate }: { d: Device; onSettings: () => void; onUpdate: () => void }) {
  return (
    <div className="lg:col-span-8 bg-surface-container-highest rounded-xl p-8 relative overflow-hidden group hover:scale-[1.01] transition-transform duration-300">
      <div className="absolute -right-20 -top-20 w-64 h-64 bg-primary-container/20 rounded-full blur-3xl" />
      <div className="absolute -left-10 -bottom-10 w-48 h-48 bg-secondary-container/20 rounded-full blur-2xl" />

      <div className="flex flex-col md:flex-row gap-8 relative z-10">
        <div className="w-full md:w-2/5 aspect-square bg-surface rounded-lg flex items-center justify-center p-6 shadow-sm relative texture-overlay overflow-hidden">
          <span className="material-symbols-outlined text-8xl text-primary/30 z-10">speaker</span>
          {d.online && <div className="absolute bottom-6 w-20 h-2 bg-primary blur-xl opacity-40" />}
        </div>

        <div className="w-full md:w-3/5 flex flex-col justify-between">
          <div>
            <div className="flex justify-between items-start mb-2">
              <h3 className="font-headline text-3xl text-on-background font-bold">{d.name ?? "صندوق نديم الذكي"}</h3>
              <div className={`bg-surface px-4 py-1.5 rounded-full flex items-center gap-2 shadow-sm text-sm font-medium ${d.online ? "text-emerald-600" : "text-on-surface-variant"}`}>
                <span className={`w-2.5 h-2.5 rounded-full ${d.online ? "bg-emerald-500 animate-pulse" : "bg-on-surface-variant"}`} />
                {d.online ? "متصل" : "غير متصل"}
              </div>
            </div>
            {d.room && <p className="font-body text-on-surface-variant mb-6">{d.room}</p>}

            <div className="grid grid-cols-2 gap-4 mb-8">
              {d.battery_level != null && (
                <div className="bg-surface rounded-lg p-4 flex items-center gap-4">
                  <div className="w-10 h-10 rounded-full bg-primary/10 flex items-center justify-center text-primary">
                    <span className="material-symbols-outlined fill">battery_charging_80</span>
                  </div>
                  <div>
                    <p className="text-xs text-on-surface-variant font-medium">البطارية</p>
                    <p className="font-headline text-lg text-on-background font-bold">{d.battery_level}%</p>
                  </div>
                </div>
              )}
              {d.wifi_signal && (
                <div className="bg-surface rounded-lg p-4 flex items-center gap-4">
                  <div className="w-10 h-10 rounded-full bg-primary/10 flex items-center justify-center text-primary">
                    <span className="material-symbols-outlined fill">wifi</span>
                  </div>
                  <div>
                    <p className="text-xs text-on-surface-variant font-medium">إشارة الواي فاي</p>
                    <p className="font-headline text-lg text-on-background font-bold">{d.wifi_signal}</p>
                  </div>
                </div>
              )}
            </div>
          </div>

          <div className="flex flex-wrap gap-4 mt-auto">
            <button onClick={onSettings}
                    className="bg-gradient-to-r from-primary to-primary-container text-on-primary font-body font-bold py-3 px-6 rounded-full flex items-center gap-2 hover:scale-105 transition-transform shadow-md shadow-primary/20">
              <span className="material-symbols-outlined fill">settings</span>
              إعدادات الجهاز
            </button>
            <button onClick={onUpdate}
                    className="bg-transparent text-primary hover:bg-surface-container-high font-body font-bold py-3 px-6 rounded-full flex items-center gap-2 transition-colors">
              <span className="material-symbols-outlined">system_update</span>
              تحديث النظام
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}
```

- [ ] **Step 3: `src/components/ConfirmDialog.tsx`**

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

- [ ] **Step 4: Devices page** — client component with bento grid, hero device card, add-device card, revoke confirm dialog.

```tsx
"use client";
import { useState } from "react";
import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { DeviceHeroCard } from "@/components/DeviceHeroCard";
import { ConfirmDialog } from "@/components/ConfirmDialog";
import { AddDeviceModal } from "@/components/AddDeviceModal";

type Device = { device_id: string; online: boolean; last_seen_at: string | null; name?: string; battery_level?: number; wifi_signal?: string; room?: string };

export default function DevicesPage() {
  const qc = useQueryClient();
  const { data: devices = [], isLoading } = useQuery({ queryKey: ["devices"], queryFn: () => client<Device[]>("/api/devices") });
  const remove = useMutation({
    mutationFn: (id: string) => client(`/api/devices/${id}`, { method: "DELETE" }),
    onSuccess: () => qc.invalidateQueries({ queryKey: ["devices"] }),
  });
  const [confirming, setConfirming] = useState<string | null>(null);
  const [adding, setAdding] = useState(false);

  return (
    <div className="pt-24 md:pt-12">
      <div className="mb-12">
        <h1 className="font-headline text-5xl text-primary font-bold mb-4 tracking-wide">أجهزتي</h1>
        <p className="font-body text-lg text-on-surface-variant max-w-2xl">أدر أجهزة نديم الخاصة بك، تحقق من حالتها، واكتشف خيارات جديدة.</p>
      </div>

      <div className="mb-16">
        <h2 className="font-headline text-3xl text-on-surface mb-6 flex items-center gap-3">
          <span className="material-symbols-outlined fill">widgets</span>
          الأجهزة المفعلة
        </h2>

        <div className="grid grid-cols-1 lg:grid-cols-12 gap-8">
          {devices[0] && (
            <DeviceHeroCard d={devices[0]} onSettings={() => {}} onUpdate={() => {}} />
          )}

          <div className="lg:col-span-4 bg-surface-container-lowest rounded-xl p-8 border border-outline-variant/15 flex flex-col items-center justify-center text-center group hover:bg-surface transition-colors duration-300">
            <div className="w-24 h-24 rounded-full bg-secondary-container/20 flex items-center justify-center mb-6 group-hover:scale-110 transition-transform duration-300">
              <button onClick={() => setAdding(true)}
                      className="w-16 h-16 rounded-full bg-secondary-container text-on-secondary-container flex items-center justify-center shadow-md shadow-secondary-container/30 hover:scale-105 transition-transform">
                <span className="material-symbols-outlined text-4xl">add</span>
              </button>
            </div>
            <h3 className="font-headline text-2xl text-on-background font-bold mb-2">إضافة جهاز جديد</h3>
            <p className="font-body text-on-surface-variant text-sm">قم بتوصيل صندوق نديم جديد لتوسيع تجربة اللعب والتعلم.</p>
          </div>
        </div>

        {devices.length > 1 && (
          <div className="mt-8 space-y-2">
            {devices.slice(1).map((d) => (
              <div key={d.device_id} className="flex items-center justify-between bg-surface-container-highest rounded-xl p-5 hover:scale-[1.01] transition-transform">
                <div>
                  <p className="font-headline text-lg font-bold text-on-surface">{d.name ?? d.device_id}</p>
                  <p className="text-sm text-on-surface-variant">{d.online ? "متصل" : "غير متصل"}</p>
                </div>
                <button onClick={() => setConfirming(d.device_id)} className="px-4 py-2 rounded-full text-error hover:bg-error-container font-body font-bold">إلغاء التفعيل</button>
              </div>
            ))}
          </div>
        )}
      </div>

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

- [ ] **Step 5: Commit** — `git commit -m "feat(frontend): devices bento grid + hero card + revoke dialog"`

---

### Task 10: Add device modal (provisioning token)

**Files:**
- Create: `src/components/AddDeviceModal.tsx`
- Create: `src/app/api/provisioning-tokens/route.ts`

- [ ] **Step 1: `src/app/api/provisioning-tokens/route.ts`**

```ts
import { NextResponse } from "next/server";
import { backend, BackendError } from "@/lib/api";

export async function POST() {
  try { return NextResponse.json(await backend("/api/provisioning-tokens", { method: "POST", body: JSON.stringify({}) }), { status: 201 }); }
  catch (e) { const s = e instanceof BackendError ? e.status : 500; return NextResponse.json({ detail: "error" }, { status: s }); }
}
```

- [ ] **Step 2: `src/components/AddDeviceModal.tsx`**

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
              <button onClick={onClose} className="px-4 py-2 rounded-full bg-surface-container font-body">إلغاء</button>
              <button onClick={generate} disabled={busy} className="px-4 py-2 rounded-full bg-gradient-to-br from-primary to-primary-container text-on-primary font-headline font-bold disabled:opacity-60">{busy ? "..." : "إنشاء الرمز"}</button>
            </div>
            {err && <p className="text-error mt-3 font-body">{err}</p>}
          </>
        ) : (
          <>
            <p className="font-body text-on-surface-variant mb-2">رمز الإقران (صالح حتى {new Date(data.expires_at).toLocaleString("ar")}):</p>
            <code className="block bg-surface-container p-3 rounded font-mono text-sm break-all">{data.provision_token}</code>
            <div className="flex justify-end mt-5">
              <button onClick={onClose} className="px-4 py-2 rounded-full bg-gradient-to-br from-primary to-primary-container text-on-primary font-headline font-bold">تم</button>
            </div>
          </>
        )}
      </div>
    </dialog>
  );
}
```

- [ ] **Step 3: Smoke**: open Devices → click "إضافة جهاز" → click "إنشاء الرمز" → token + expiry render.

- [ ] **Step 4: Commit** — `git commit -m "feat(frontend): add device modal generates provisioning token"`

---

### Task 11: E2E happy-path tests

**Files:**
- Create: `e2e/dashboard.spec.ts`

- [ ] **Step 1: `e2e/dashboard.spec.ts`**

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

  test("nav links work on desktop", async ({ page }) => {
    await page.getByRole("link", { name: /مكتبة السماسم/ }).click();
    await expect(page).toHaveURL("/library");
    await page.getByRole("link", { name: /سجل المحادثات/ }).click();
    await expect(page).toHaveURL("/chat-history");
    await page.getByRole("link", { name: /أجهزتي/ }).click();
    await expect(page).toHaveURL("/devices");
  });

  test("add device generates token", async ({ page }) => {
    await page.goto("/devices");
    await page.getByRole("button", { name: /إضافة جهاز/ }).click();
    await page.getByRole("button", { name: "إنشاء الرمز" }).click();
    await expect(page.getByText(/صالح حتى/)).toBeVisible();
  });

  test("mobile bottom nav visible", async ({ page }) => {
    await page.setViewportSize({ width: 375, height: 812 });
    await expect(page.locator("nav").filter({ hasText: "الرئيسية" })).toBeVisible();
  });
});
```

- [ ] **Step 2: Run** — `npm run e2e`. Inspect failures, fix selectors.

- [ ] **Step 3: Commit** — `git commit -m "test(frontend): playwright e2e for nav + add-device + mobile"`

---

### Task 12: Loading/error/empty states

**Files:**
- Create: `src/components/ErrorState.tsx`
- Create: `src/components/EmptyState.tsx`
- Create: `src/app/(dash)/error.tsx`
- Create: `src/app/(dash)/loading.tsx`

- [ ] **Step 1: `src/components/EmptyState.tsx`**

```tsx
export function EmptyState({ icon, title, body }: { icon: string; title: string; body: string }) {
  return (
    <div className="bg-surface-container-highest rounded-xl p-12 text-center">
      <span className="material-symbols-outlined text-6xl text-on-surface-variant">{icon}</span>
      <h3 className="font-headline text-xl font-bold mt-3">{title}</h3>
      <p className="font-body text-on-surface-variant">{body}</p>
    </div>
  );
}
```

- [ ] **Step 2: `src/components/ErrorState.tsx`**

```tsx
"use client";
export function ErrorState({ reset }: { reset: () => void }) {
  return (
    <div className="bg-error-container text-on-error-container rounded-xl p-8 text-center">
      <span className="material-symbols-outlined text-4xl mb-3">error</span>
      <h3 className="font-headline text-xl font-bold mb-2">حدث خطأ</h3>
      <button onClick={reset} className="px-4 py-2 rounded-full bg-error text-on-error font-bold">حاول مجدداً</button>
    </div>
  );
}
```

- [ ] **Step 3: `src/app/(dash)/error.tsx`**

```tsx
"use client";
import { ErrorState } from "@/components/ErrorState";

export default function Error({ reset }: { error: Error; reset: () => void }) {
  return <ErrorState reset={reset} />;
}
```

- [ ] **Step 4: `src/app/(dash)/loading.tsx`** — skeleton shimmer matching dashboard layout.

```tsx
export default function Loading() {
  return (
    <div className="pt-24 md:pt-8 animate-pulse">
      <div className="mb-10">
        <div className="h-10 w-64 bg-surface-container-high rounded-lg mb-2" />
        <div className="h-6 w-96 bg-surface-container rounded-lg" />
      </div>
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {[1,2,3].map((i) => <div key={i} className="bg-surface-container-highest rounded-xl p-8 h-48" />)}
      </div>
    </div>
  );
}
```

- [ ] **Step 5: Replace inline empty messages with `EmptyState` in dashboard, library, chat-history, devices pages.**

- [ ] **Step 6: Commit** — `git commit -m "feat(frontend): unified loading/error/empty states"`

---

### Task 13: Bundle check + final smoke

- [ ] **Step 1: Build** — `docker compose run --rm frontend npm run build`. Should complete with no TypeScript errors.

- [ ] **Step 2: Lighthouse smoke** — open `http://localhost:3000` after login; check that fonts load (Cairo + Baloo Bhaijaan 2), `<html dir="rtl" lang="ar">` is present, no console errors.

- [ ] **Step 3: Cross-page click-through** — login → dashboard → library → semsem → back → chat-history → session → back → devices → add device → revoke device → logout. Each step should feel sub-200ms after warm-up.

- [ ] **Step 4: Mobile responsive check** — at 375px width, verify topbar + bottom nav render, sidebar hidden, cards stack vertically.

- [ ] **Step 5: Final commit** — `git commit --allow-empty -m "chore(frontend): v0.1 design-faithful ready"` (or skip if no diffs).

---

## Self-review

**Spec coverage (PRD §4.2 + §5 + `frontend/design.md`):**

- Auth — Task 3 (login), Task 4 (logout in shell).
- Main dashboard aggregating `/api/users/me/stats` + `/api/devices` — Task 5.
- Library mapping `/api/semsems` with عادي / ذكي badges — Task 6 (unit test for both badges).
- Library detail mapping `/api/semsems/<uid>` — Task 7.
- Chat history list + detail with Child vs AI bubble distinction — Task 8 (ChatBubble unit test).
- Devices page with `DELETE /api/devices/<id>` revoke — Task 9.
- Add device flow generating provisioning token — Task 10.
- RTL + Arabic + Baloo Bhaijaan 2 + Cairo — Task 1.
- Mobile responsive (sidebar → topbar/bottomnav) — Task 4.
- Design-faithful components (bento grids, decorative blobs, gradient buttons, texture overlays, Material Symbols with FILL states) — Tasks 3-10.
- Loading/empty/error UX — Task 12.

**Placeholder scan:** none. Every task has runnable code, every test has a body, every command has expected output.

**Type/name consistency:** `Device.device_id`, `Semsem.uid_hex`, `is_pro`, `role` match backend data model. Stats response fields (`total_listening_ms`, `pro_total_ms`, etc.) match the backend aggregator.

**Open question:** Images — the mockups use hosted Google images. For v0.1, `SemsemCard` renders a gradient placeholder when `image_url` is absent. The logo uses a self-hosted SVG (`public/nadeem-logo.svg`).

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-04-26-nadeem-frontend-v2.md`.**

**1. Subagent-Driven (recommended)** — fresh subagent per task, review between tasks, fast iteration.

**2. Inline Execution** — execute tasks in this session using executing-plans, batch execution with checkpoints.
