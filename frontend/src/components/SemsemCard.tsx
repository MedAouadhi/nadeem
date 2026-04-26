import React from "react";
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
