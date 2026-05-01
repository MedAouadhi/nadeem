"use client";
import { useState } from "react";
import { useMutation, useQueryClient } from "@tanstack/react-query";

type Props = { open: boolean; onClose: () => void };

export function AddDeviceModal({ open, onClose }: Props) {
  const qc = useQueryClient();
  const [token, setToken] = useState<string | null>(null);
  const [expiresAt, setExpiresAt] = useState<string | null>(null);
  const provision = useMutation({
    mutationFn: () =>
      fetch("/api/provisioning-tokens", { method: "POST", headers: { "Content-Type": "application/json" } }).then((r) => {
        if (!r.ok) throw new Error("Failed to create token");
        return r.json();
      }),
    onSuccess: (data) => {
      setToken(data.provision_token);
      setExpiresAt(data.expires_at);
      qc.invalidateQueries({ queryKey: ["devices"] });
    },
  });

  if (!open) return null;

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/40 backdrop-blur-sm" onClick={onClose}>
      <div className="bg-surface-container-highest rounded-2xl p-8 w-full max-w-md mx-4 shadow-2xl" onClick={(e) => e.stopPropagation()}>
        <div className="flex items-center justify-between mb-6">
          <h2 className="font-headline text-2xl font-bold text-primary">إضافة جهاز جديد</h2>
          <button onClick={onClose} className="text-on-surface-variant hover:text-primary">
            <span className="material-symbols-outlined">close</span>
          </button>
        </div>

        {token ? (
          <div className="text-center">
            <div className="w-16 h-16 rounded-full bg-primary-container/20 flex items-center justify-center mx-auto mb-4">
              <span className="material-symbols-outlined text-3xl text-primary">check_circle</span>
            </div>
            <p className="font-body text-on-surface mb-2">تم إنشاء رمز التفعيل. أدخله على الجهاز:</p>
            {expiresAt && (
              <p className="text-xs text-on-surface-variant mb-4">
                صالح حتى: {new Date(expiresAt).toLocaleString("ar-SA")}
              </p>
            )}
            <div className="bg-surface-container rounded-xl p-4 mb-6 font-mono text-lg text-primary text-center tracking-widest select-all break-all" dir="ltr">
              {token}
            </div>
            <button onClick={() => { setToken(null); setExpiresAt(null); onClose(); }}
                    className="px-6 py-3 bg-gradient-to-l from-primary to-primary-container text-on-primary rounded-full font-bold shadow-md hover:scale-[1.02] transition-all">
              تم
            </button>
          </div>
        ) : (
          <div>
            <p className="font-body text-on-surface-variant mb-6">
              انقر على "تفعيل" لإنشاء رمز تفعيل مؤقت. أدخل هذا الرمز على جهاز نديم لربطه بحسابك.
            </p>
            <div className="flex gap-3 justify-end">
              <button type="button" onClick={onClose} className="px-6 py-3 rounded-full font-bold text-on-surface-variant hover:bg-surface-container-high transition-colors">
                إلغاء
              </button>
              <button type="button" onClick={() => provision.mutate()} disabled={provision.isPending}
                      className="px-6 py-3 bg-gradient-to-l from-primary to-primary-container text-on-primary rounded-full font-bold shadow-md hover:scale-[1.02] transition-all disabled:opacity-50">
                {provision.isPending ? "جارٍ..." : "تفعيل"}
              </button>
            </div>
            {provision.isError && (
              <p className="text-error font-body text-sm mt-4 text-center">{(provision.error as Error).message}</p>
            )}
          </div>
        )}
      </div>
    </div>
  );
}
