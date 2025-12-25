import { Geist, Geist_Mono } from "next/font/google";
import "@/styles/globals.css";
import { ThemeProvider } from "@/components/ThemeProvider";
import { Sidebar } from "@/components/Sidebar";
import { Header } from "@/components/Header";
import { Search } from "@/components/Search";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export const metadata = {
  title: "NetworkLib Documentation",
  description: "A high-performance C++20 network library.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" suppressHydrationWarning>
      <body
        className={`${geistSans.variable} ${geistMono.variable} antialiased min-h-screen bg-background text-foreground`}
      >
        <ThemeProvider
          attribute="data-theme"
          defaultTheme="dark"
          enableSystem={false}
          themes={['dark', 'light', 'high-contrast', 'oceanic', 'sunset', 'cyberpunk']}
        >
          <div className="flex min-h-screen">
             <Sidebar />
             <main className="flex-1 lg:pl-[280px] flex flex-col min-h-screen transition-all duration-300">
                <Header />
                <div className="flex-1 p-6 lg:p-10 relative overflow-hidden">
                   {children}
                </div>
             </main>
             <Search />
          </div>
        </ThemeProvider>
      </body>
    </html>
  );
}
