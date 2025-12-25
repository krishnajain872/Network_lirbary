"use client";

import "./globals.css";
import { Sidebar } from "@/components/Layout/Sidebar";
import { Navbar } from "@/components/Layout/Navbar";
import AnimatedBackground from "@/components/Background/AnimatedBackground";
import { useState } from "react";
import { ThemeProvider } from "@/components/ThemeProvider";

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  const [sidebarOpen, setSidebarOpen] = useState(false);

  return (
    <html lang="en" suppressHydrationWarning>
      <body>
        <ThemeProvider attribute="class" defaultTheme="system" enableSystem disableTransitionOnChange>
            <div className="min-h-screen relative flex flex-col">
              <AnimatedBackground />
              <Navbar onMenuClick={() => setSidebarOpen(true)} />

              <div className="flex flex-1 pt-16">
                <Sidebar isOpen={sidebarOpen} setIsOpen={setSidebarOpen} />

                <main className="flex-1 md:ml-64 w-full min-h-[calc(100vh-4rem)] relative z-10">
                  {children}
                </main>
              </div>
            </div>
        </ThemeProvider>
      </body>
    </html>
  );
}
