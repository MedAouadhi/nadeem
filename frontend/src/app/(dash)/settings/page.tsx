"use client";
import { useState } from "react";
import { useRouter } from "next/navigation";

export default function SettingsPage() {
  const router = useRouter();
  const [language, setLanguage] = useState("ar");
  const [notifications, setNotifications] = useState(true);
  const [busy, setBusy] = useState(false);

  async function logout() {
    setBusy(true);
    try {
      await fetch("/api/auth/logout", { method: "DELETE" });
      router.push("/login");
      router.refresh();
    } catch {
      // ignore
    } finally {
      setBusy(false);
    }
  }

  return (
    <div className="pt-24 md:pt-8 max-w-2xl">
      <header className="mb-10">
        <h1 className="font-headline text-4xl font-bold text-primary mb-2">الإعدادات</h1>
        <p className="text-on-surface-variant">تخصيص تجربة نديم</p>
      </header>

      <div className="space-y-6">
        {/* Language */}
        <div className="bg-surface-container-highest rounded-xl p-6">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-4">
              <div className="w-10 h-10 rounded-xl bg-primary-container/20 flex items-center justify-center">
                <span className="material-symbols-outlined text-primary">language</span>
              </div>
              <div>
                <h3 className="font-bold text-on-surface">اللغة</h3>
                <p className="text-sm text-on-surface-variant">لغة واجهة التطبيق</p>
              </div>
            </div>
            <select
              value={language}
              onChange={(e) => setLanguage(e.target.value)}
              className="bg-surface-container rounded-lg px-4 py-2 text-on-surface font-body focus:outline-none focus:ring-2 focus:ring-primary"
            >
              <option value="ar">العربية</option>
              <option value="en">English</option>
            </select>
          </div>
        </div>

        {/* Notifications */}
        <div className="bg-surface-container-highest rounded-xl p-6">
          <div className="flex items-center justify-between">
            <div className="flex items-center gap-4">
              <div className="w-10 h-10 rounded-xl bg-secondary-container/20 flex items-center justify-center">
                <span className="material-symbols-outlined text-secondary">notifications</span>
              </div>
              <div>
                <h3 className="font-bold text-on-surface">الإشعارات</h3>
                <p className="text-sm text-on-surface-variant">تلقي إشعارات نشاط الأجهزة</p>
              </div>
            </div>
            <button
              onClick={() => setNotifications(!notifications)}
              className={`w-14 h-8 rounded-full transition-colors relative ${notifications ? "bg-primary" : "bg-surface-container"}`}
            >
              <div className={`absolute top-1 w-6 h-6 rounded-full bg-on-primary shadow-sm transition-all ${notifications ? "left-7" : "left-1"}`} />
            </button>
          </div>
        </div>

        {/* Account */}
        <div className="bg-surface-container-highest rounded-xl p-6">
          <div className="flex items-center gap-4 mb-4">
            <div className="w-10 h-10 rounded-xl bg-tertiary-container/20 flex items-center justify-center">
              <span className="material-symbols-outlined text-tertiary">person</span>
            </div>
            <div>
              <h3 className="font-bold text-on-surface">الحساب</h3>
              <p className="text-sm text-on-surface-variant">إدارة حسابك</p>
            </div>
          </div>
          <button
            onClick={logout}
            disabled={busy}
            className="w-full py-3 rounded-xl border border-error text-error font-bold hover:bg-error/5 transition-colors disabled:opacity-50"
          >
            {busy ? "جارٍ..." : "تسجيل الخروج"}
          </button>
        </div>

        {/* About */}
        <div className="bg-surface-container-highest rounded-xl p-6">
          <div className="flex items-center gap-4">
            <div className="w-10 h-10 rounded-xl bg-surface-container flex items-center justify-center">
              <span className="material-symbols-outlined text-on-surface-variant">info</span>
            </div>
            <div>
              <h3 className="font-bold text-on-surface">عن نديم</h3>
              <p className="text-sm text-on-surface-variant">الإصدار 0.1.0</p>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
