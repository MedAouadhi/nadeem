import { backend } from "@/lib/api";
import { StatCard } from "@/components/StatCard";
import { ErrorCard } from "@/components/ErrorCard";
import { msToMinutes, presenceCopy } from "@/lib/format";

type Stats = {
  today_listening_ms: number;
  total_listening_ms: number;
  today_pro_ms: number;
  total_pro_ms: number;
  unique_semsems: number;
  device_count: number;
  online_device_count: number;
};
type Device = {
  device_id: string;
  online: boolean;
  last_seen_at: string | null;
  name: string | null;
  today_listening_ms: number;
  total_listening_ms: number;
  total_semsems: number;
};

export default async function Dashboard() {
  let stats: Stats | null = null;
  let devices: Device[] = [];

  try {
    [stats, devices] = await Promise.all([
      backend<Stats>("/api/users/me/stats"),
      backend<Device[]>("/api/devices"),
    ]);
  } catch {
    return <ErrorCard />;
  }

  if (!stats) return <ErrorCard />;

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
          label="دقائق الاستماع اليوم"
          value={msToMinutes(stats.today_listening_ms)}
          sub="دقيقة"
          iconBg="bg-primary-container/20"
          decorative="bg-primary/5"
        />
        <StatCard
          icon="schedule"
          label="إجمالي دقائق الاستماع"
          value={msToMinutes(stats.total_listening_ms)}
          sub="دقيقة"
          iconBg="bg-secondary-container/20"
          decorative="bg-secondary-container/10"
        />
        <StatCard
          icon="smart_toy"
          label="السماسم المملوكة"
          value={stats.unique_semsems}
          sub="سماسم"
          iconBg="bg-tertiary-container/20"
          decorative="bg-tertiary-container/10"
        />
        <StatCard
          icon="wifi"
          label="حالة الجهاز"
          value={primaryDevice ? (primaryDevice.name ?? "جهاز نديم") : "لا يوجد جهاز"}
          iconBg="bg-surface-container"
          decorative="bg-primary/5"
          badge={
            primaryDevice ? (
              <div className="flex items-center gap-2 bg-surface px-3 py-1.5 rounded-full shadow-sm">
                <div className={`w-2.5 h-2.5 rounded-full ${primaryDevice.online ? "bg-green-500 animate-pulse" : "bg-on-surface-variant"}`} />
                <span className={`text-xs font-bold ${primaryDevice.online ? "text-green-700" : "text-on-surface-variant"}`}>
                  {presenceCopy(primaryDevice.online)}
                </span>
              </div>
            ) : undefined
          }
        />
      </div>
    </div>
  );
}
