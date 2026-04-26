import { redirect } from "next/navigation";
import { backend, BackendError } from "@/lib/api";
import { Sidebar } from "@/components/Sidebar";
import { TopBar } from "@/components/TopBar";
import { BottomNav } from "@/components/BottomNav";
import { QueryProvider } from "@/lib/query-client";

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

function DashShell({ email, children }: { email: string; children: React.ReactNode }) {
  return (
    <div className="min-h-screen bg-background text-on-background">
      <Sidebar onAddDevice={() => {}} />
      <TopBar />
      <main className="md:mr-72 pt-0 md:pt-0 px-6 md:px-12 pb-24 md:pb-12 max-w-7xl mx-auto w-full">
        {children}
      </main>
      <BottomNav />
    </div>
  );
}
