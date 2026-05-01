export function msToHours(ms: number): string {
  const h = ms / 3_600_000;
  return new Intl.NumberFormat("ar", { maximumFractionDigits: 1 }).format(h);
}

export function msToMinutes(ms: number): string {
  return new Intl.NumberFormat("ar", { maximumFractionDigits: 0 }).format(ms / 60_000);
}

export function fmtDate(iso: string): string {
  return new Intl.DateTimeFormat("ar", { dateStyle: "long", timeStyle: "short" }).format(new Date(iso));
}

export function fmtRelative(iso: string): string {
  const now = Date.now();
  const then = new Date(iso).getTime();
  const diffDays = Math.floor((now - then) / 86_400_000);
  if (diffDays === 0) return "اليوم";
  if (diffDays === 1) return "أمس";
  return new Intl.DateTimeFormat("ar", { dateStyle: "medium" }).format(new Date(iso));
}

export function presenceCopy(online: boolean): string {
  return online ? "نشط خلال آخر 5 دقائق" : "غير نشط حالياً";
}
