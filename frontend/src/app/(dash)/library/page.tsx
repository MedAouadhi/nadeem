"use client";
import { useQuery } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { SemsemCard } from "@/components/SemsemCard";
import { ErrorCard } from "@/components/ErrorCard";
import { useState } from "react";

type Semsem = { uid_hex: string; title: string; is_pro: boolean; role: string; description?: string; duration?: string; image_url?: string };

export default function Library() {
  const [filter, setFilter] = useState<"all" | "regular" | "pro">("all");
  const { data: list = [], isLoading, error } = useQuery({ queryKey: ["semsems"], queryFn: () => client<Semsem[]>("/api/semsems") });

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

      {error ? (
        <ErrorCard />
      ) : isLoading ? (
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-8">
          {[1,2,3,4].map((i) => <div key={i} className="bg-surface-container-highest rounded-[2rem] h-72 animate-pulse" />)}
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
