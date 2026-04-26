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
