import { redirect } from "next/navigation";
import { backend, BackendError } from "@/lib/api";
import { QueryProvider } from "@/lib/query-client";
import DashShell from "@/components/DashShell";

type Me = { id: number; email: string };

export default async function DashLayout({ children }: { children: React.ReactNode }) {
  let me: Me;
  try {
    me = await backend<Me>("/api/auth/me");
  } catch (e) {
    if (e instanceof BackendError && e.status === 401) redirect("/login");
    throw e;
  }
  return (
    <QueryProvider>
      <DashShell email={me.email}>{children}</DashShell>
    </QueryProvider>
  );
}
