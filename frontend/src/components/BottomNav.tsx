"use client";
import Link from "next/link";
import { usePathname } from "next/navigation";

const TABS = [
  { href: "/", label: "الرئيسية", icon: "home" },
  { href: "/devices", label: "أجهزتي", icon: "devices" },
  { href: "/library", label: "المكتبة", icon: "library_music" },
  { href: "/chat", label: "السجل", icon: "forum" },
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
