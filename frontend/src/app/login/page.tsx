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
