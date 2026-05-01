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
