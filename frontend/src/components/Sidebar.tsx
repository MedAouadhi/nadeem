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
