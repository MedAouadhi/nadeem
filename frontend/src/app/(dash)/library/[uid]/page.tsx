import { backend } from "@/lib/api";
import { StatCard } from "@/components/StatCard";
import { msToMinutes } from "@/lib/format";
import Link from "next/link";

type SemsemDetail = {
  uid_hex: string;
  title: string;
  is_pro: boolean;
  role: string;
  description?: string;
  image_url?: string;
  today_play_count?: number;
  total_play_count?: number;
  today_listening_ms?: number;
  total_listening_ms?: number;
  last_played_at?: string;
};

export default async function SemsemDetailPage({ params }: { params: Promise<{ uid: string }> }) {
  const { uid } = await params;
  const semsem = await backend<SemsemDetail>(`/api/semsems/${uid}`);

  return (
    <div className="pt-24 md:pt-8">
      <Link href="/library" className="inline-flex items-center gap-2 text-on-surface-variant hover:text-primary transition-colors mb-8">
        <span className="material-symbols-outlined">arrow_forward</span>
        <span className="font-body">العودة للمكتبة</span>
      </Link>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-8 mb-12">
        <div className={`relative bg-surface-container-highest rounded-[2rem] overflow-hidden h-80 ${semsem.is_pro ? "ring-2 ring-primary/20" : ""}`}>
          {semsem.is_pro && <div className="absolute inset-0 bg-gradient-to-tr from-primary/20 to-transparent mix-blend-overlay z-10" />}
          {semsem.image_url ? (
            <img alt={semsem.title} src={semsem.image_url} className="w-full h-full object-cover" />
          ) : (
            <div className="w-full h-full flex items-center justify-center bg-gradient-to-br from-primary-container/20 to-surface-container">
              <span className="material-symbols-outlined text-[8rem] text-primary/20">
                {semsem.is_pro ? "smart_toy" : "volume_up"}
              </span>
            </div>
          )}
          <div className={`absolute top-6 start-6 px-4 py-1.5 rounded-full flex items-center gap-1.5 shadow-sm z-20
            ${semsem.is_pro ? "bg-tertiary-container text-on-tertiary-container" : "bg-surface/90 backdrop-blur-md"}`}>
            <span className={`material-symbols-outlined text-sm fill ${semsem.is_pro ? "" : "text-primary"}`}>{semsem.is_pro ? "smart_toy" : "volume_up"}</span>
            <span className={`text-sm font-bold ${semsem.is_pro ? "" : "text-primary"}`}>{semsem.is_pro ? "ذكي" : "عادي"}</span>
          </div>
        </div>

        <div className="flex flex-col justify-center">
          <h1 className="font-headline text-4xl font-bold text-primary mb-3">{semsem.title}</h1>
          <p className="text-on-surface-variant text-lg mb-6 leading-relaxed">{semsem.description ?? (semsem.is_pro ? "شخصية تفاعلية ذكية" : "محتوى صوتي")}</p>
          <div className="flex items-center gap-3">
            <span className="px-4 py-2 rounded-full bg-secondary-container text-on-secondary-container font-bold text-sm">{semsem.role}</span>
          </div>
        </div>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        <StatCard
          icon="play_circle"
          label="عدد مرات التشغيل"
          value={semsem.total_play_count ?? 0}
          sub="مرة"
          iconBg="bg-primary-container/20"
          decorative="bg-primary/5"
        />
        <StatCard
          icon="schedule"
          label="دقائق الاستماع اليوم"
          value={msToMinutes(semsem.today_listening_ms ?? 0)}
          sub="دقيقة"
          iconBg="bg-secondary-container/20"
          decorative="bg-secondary-container/10"
        />
        <StatCard
          icon="headphones"
          label="إجمالي دقائق الاستماع"
          value={msToMinutes(semsem.total_listening_ms ?? 0)}
          sub="دقيقة"
          iconBg="bg-tertiary-container/20"
          decorative="bg-tertiary-container/10"
        />
      </div>

      <div className="mt-6">
        <StatCard
          icon="access_time"
          label="آخر تشغيل"
          value={semsem.last_played_at ? new Date(semsem.last_played_at).toLocaleDateString("ar-SA") : "لم يُشغّل"}
          iconBg="bg-surface-container"
          decorative="bg-primary/5"
        />
      </div>
    </div>
  );
}
