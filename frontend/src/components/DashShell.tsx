"use client";
import { useState } from "react";
import { Sidebar } from "@/components/Sidebar";
import { TopBar } from "@/components/TopBar";
import { BottomNav } from "@/components/BottomNav";
import { AddDeviceModal } from "@/components/AddDeviceModal";

export default function DashShell({ email, children }: { email: string; children: React.ReactNode }) {
  const [showAddDevice, setShowAddDevice] = useState(false);

  return (
    <div className="min-h-screen bg-background text-on-background">
      <Sidebar onAddDevice={() => setShowAddDevice(true)} />
      <TopBar />
      <main className="md:mr-72 pt-0 md:pt-0 px-6 md:px-12 pb-24 md:pb-12 max-w-7xl mx-auto w-full">
        {children}
      </main>
      <BottomNav />
      <AddDeviceModal open={showAddDevice} onClose={() => setShowAddDevice(false)} />
    </div>
  );
}
