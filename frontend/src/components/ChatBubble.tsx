type Props = {
  role: "user" | "assistant";
  text: string;
  ts: string;
};

export function ChatBubble({ role, text, ts }: Props) {
  const isUser = role === "user";
  return (
    <div className={`flex ${isUser ? "justify-start" : "justify-end"} mb-4`}>
      <div className={`max-w-[75%] px-5 py-3 rounded-[1.25rem] shadow-sm ${
        isUser
          ? "bg-primary text-on-primary rounded-bl-sm"
          : "bg-surface-container-highest text-on-surface rounded-be-sm"
      }`}>
        <p className="font-body text-sm leading-relaxed whitespace-pre-wrap">{text}</p>
        <p className={`text-[0.65rem] mt-1.5 ${isUser ? "text-on-primary/60" : "text-on-surface-variant"}`}>
          {new Date(ts).toLocaleTimeString("ar-SA", { hour: "2-digit", minute: "2-digit" })}
        </p>
      </div>
    </div>
  );
}
