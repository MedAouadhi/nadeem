"use client";
import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { useState } from "react";
import { AddDeviceModal } from "@/components/AddDeviceModal";
import { ErrorCard } from "@/components/ErrorCard";
import { msToMinutes, presenceCopy } from "@/lib/format";

type Device = {
  device_id: string;
  name: string | null;
  online: boolean;
  last_seen_at: string | null;
  firmware_version: string | null;
  today_listening_ms: number;
  total_listening_ms: number;
  total_semsems: number;
};

export default function Devices() {
  const qc = useQueryClient();
  const [revoking, setRevoking] = useState<string | null>(null);
  const [showAdd, setShowAdd] = useState(false);
  const { data: devices = [], isLoading, error } = useQuery({ queryKey: ["devices"], queryFn: () => client<Device[]>("/api/devices") });
  const revoke = useMutation({
    mutationFn: (id: string) => fetch(`/api/devices/${id}`, { method: "DELETE" }),
    onSuccess: () => qc.invalidateQueries({ queryKey: ["devices"] }),
  });

  const primary = devices[0];
  const others = devices.slice(1);

  return (
    <div className="pt-24 md:pt-8">
      <div className="flex items-center justify-between mb-10">
        <div>
          <h1 className="font-headline text-4xl font-bold text-primary mb-2">الأجهزة</h1>
          <p className="text-on-surface-variant">إدارة أجهزة نديم المسجلة</p>
        </div>
        <button 
          onClick={() => setShowAdd(true)}
          className="px-6 py-3 bg-gradient-to-l from-primary to-primary-container text-on-primary rounded-full font-bold shadow-md hover:shadow-lg hover:scale-[1.02] transition-all flex items-center gap-2"
        >
          <span className="material-symbols-outlined">add</span>
          إضافة جهاز
        </button>
      </div>

      {error ? (
        <ErrorCard />
      ) : isLoading ? (
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          {[1,2,3].map((i) => <div key={i} className="bg-surface-container-highest rounded-xl h-48 animate-pulse" />)}
        </div>
      ) : devices.length === 0 ? (
        <div className="bg-surface-container-highest rounded-xl p-12 text-center">
          <span className="material-symbols-outlined text-6xl text-on-surface-variant">devices</span>
          <h3 className="font-headline text-xl font-bold mt-3">لا توجد أجهزة</h3>
          <p className="font-body text-on-surface-variant">أضف جهاز نديم الأول</p>
        </div>
      ) : (
        <>
          {primary && (
            <div className="bg-surface-container-highest rounded-2xl p-8 mb-8 relative overflow-hidden">
              <div className="absolute -start-8 -top-8 w-40 h-40 bg-primary/5 rounded-full blur-2xl" />
              <div className="relative z-10 flex flex-col md:flex-row md:items-center md:justify-between gap-6">
                <div className="flex items-center gap-5">
                  <div className="w-16 h-16 rounded-2xl bg-primary-container/20 flex items-center justify-center">
                    <span className="material-symbols-outlined text-4xl text-primary">speaker</span>
                  </div>
                  <div>
                    <h2 className="font-headline text-2xl font-bold text-primary">{primary.name ?? "جهاز نديم"}</h2>
                    <div className="flex items-center gap-2 mt-1">
                      <div className={`w-2.5 h-2.5 rounded-full ${primary.online ? "bg-green-500 animate-pulse" : "bg-on-surface-variant"}`} />
                      <span className={`text-sm font-bold ${primary.online ? "text-green-700" : "text-on-surface-variant"}`}>
                        {presenceCopy(primary.online)}
                      </span>
                      {primary.firmware_version && <span className="text-xs text-on-surface-variant ms-3">v{primary.firmware_version}</span>}
                    </div>
                  </div>
                </div>
                <div className="grid grid-cols-2 gap-4">
                  <div className="text-center">
                    <p className="text-2xl font-bold text-primary">{primary.total_semsems}</p>
                    <p className="text-xs text-on-surface-variant">سمسم</p>
                  </div>
                  <div className="text-center">
                    <p className="text-2xl font-bold text-primary">{msToMinutes(primary.today_listening_ms)}</p>
                    <p className="text-xs text-on-surface-variant">دقائق اليوم</p>
                  </div>
                </div>
              </div>
            </div>
          )}

          {others.length > 0 && (
            <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
              {others.map((d) => (
                <div key={d.device_id} className="bg-surface-container-highest rounded-xl p-6 relative overflow-hidden group hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300">
                  <div className="flex items-center gap-3 mb-4">
                    <div className="w-10 h-10 rounded-xl bg-primary-container/20 flex items-center justify-center">
                      <span className="material-symbols-outlined text-xl text-primary">speaker</span>
                    </div>
                    <div>
                      <p className="font-bold text-on-surface">{d.name ?? "جهاز نديم"}</p>
                      <div className="flex items-center gap-1.5">
                        <div className={`w-2 h-2 rounded-full ${d.online ? "bg-green-500" : "bg-on-surface-variant"}`} />
                        <span className="text-xs text-on-surface-variant">{presenceCopy(d.online)}</span>
                      </div>
                    </div>
                  </div>
                  <button onClick={() => { setRevoking(d.device_id); revoke.mutate(d.device_id); }}
                          disabled={revoking === d.device_id}
                          className="text-error text-sm font-bold hover:underline disabled:opacity-50">
                    إلغاء الارتباط
                  </button>
                </div>
              ))}
            </div>
          )}
        </>
      )}
      <AddDeviceModal open={showAdd} onClose={() => setShowAdd(false)} />
    </div>
  );
}
