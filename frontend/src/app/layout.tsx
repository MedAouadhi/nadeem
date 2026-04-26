import "./globals.css";
import type { ReactNode } from "react";

export const metadata = { title: "نديم", description: "لوحة تحكم الآباء" };

export default function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html dir="rtl" lang="ar" className="light">
      <body className="antialiased">{children}</body>
    </html>
  );
}
