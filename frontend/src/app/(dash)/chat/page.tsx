"use client";
import { useQuery } from "@tanstack/react-query";
import { client } from "@/lib/api-client";
import { ChatBubble } from "@/components/ChatBubble";
import { useState } from "react";
import { fmtDate } from "@/lib/format";

type Session = { id: string; semsem_title: string; semsem_uid: string; started_at: string; message_count: number };
type Message = { role: "user" | "assistant"; text: string; ts: string };

export default function ChatHistory() {
  const [selected, setSelected] = useState<string | null>(null);
  const { data: sessions = [] } = useQuery({ queryKey: ["chat-sessions"], queryFn: () => client<Session[]>("/api/chat-sessions") });
  const { data: messages = [] } = useQuery({
    queryKey: ["chat-session", selected],
    queryFn: () => client<Message[]>(`/api/chat-sessions/${selected}`),
    enabled: !!selected,
  });

  return (
    <div className="pt-24 md:pt-8 h-[calc(100vh-2rem)] flex flex-col">
      <h1 className="font-headline text-4xl font-bold text-primary mb-6">سجل المحادثات</h1>

      <div className="flex-1 flex gap-6 min-h-0">
        <div className={`w-full md:w-80 shrink-0 bg-surface-container-low rounded-xl overflow-y-auto ${selected ? "hidden md:block" : ""}`}>
          {sessions.length === 0 ? (
            <div className="p-8 text-center text-on-surface-variant">
              <span className="material-symbols-outlined text-4xl mb-2">forum</span>
              <p>لا توجد محادثات بعد</p>
            </div>
          ) : sessions.map((s) => (
            <button key={s.id} onClick={() => setSelected(s.id)}
                    className={`w-full text-start p-4 hover:bg-surface-container-high transition-colors border-b border-outline-variant last:border-b-0
                      ${selected === s.id ? "bg-surface-container-high" : ""}`}>
              <div className="flex items-center gap-3">
                <div className="w-10 h-10 rounded-full bg-tertiary-container flex items-center justify-center">
                  <span className="material-symbols-outlined text-on-tertiary-container">smart_toy</span>
                </div>
                <div className="flex-1 min-w-0">
                  <p className="font-bold text-on-surface truncate">{s.semsem_title}</p>
                  <p className="text-xs text-on-surface-variant">{fmtDate(s.started_at)} · {s.message_count} رسالة</p>
                </div>
              </div>
            </button>
          ))}
        </div>

        <div className={`flex-1 bg-surface-container-low rounded-xl flex flex-col min-h-0 ${!selected ? "hidden md:flex" : ""}`}>
          {selected ? (
            <>
              <div className="p-4 border-b border-outline-variant flex items-center gap-3 md:hidden">
                <button onClick={() => setSelected(null)} className="text-on-surface-variant hover:text-primary">
                  <span className="material-symbols-outlined">arrow_forward</span>
                </button>
                <span className="font-bold text-on-surface">{sessions.find((s) => s.id === selected)?.semsem_title}</span>
              </div>
              <div className="flex-1 overflow-y-auto p-6 space-y-2">
                {messages.map((m, i) => <ChatBubble key={i} {...m} />)}
              </div>
            </>
          ) : (
            <div className="flex-1 flex items-center justify-center text-on-surface-variant">
              <div className="text-center">
                <span className="material-symbols-outlined text-6xl mb-3">chat</span>
                <p>اختر محادثة لعرضها</p>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
