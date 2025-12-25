"use client";

import Link from "next/link";
import { Sun, Moon, Menu, Zap, Eye, Monitor } from "lucide-react";
import { useTheme } from "next-themes";
import { useState, useEffect } from "react";
import { CommandMenu } from "@/components/Search/CommandMenu";

interface NavbarProps {
  onMenuClick: () => void;
}

export function Navbar({ onMenuClick }: NavbarProps) {
  const { theme, setTheme } = useTheme();
  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
  }, []);

  return (
    <header className="fixed top-0 left-0 right-0 z-50 h-16 bg-background/80 backdrop-blur-md border-b border-border flex items-center justify-between px-4 md:px-8">
      <div className="flex items-center gap-4">
        <button
          onClick={onMenuClick}
          className="md:hidden p-2 hover:bg-secondary rounded-md"
        >
          <Menu size={24} />
        </button>
        <Link href="/" className="text-xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-purple-600">
          NetworkLib
        </Link>
      </div>

      <div className="flex items-center gap-4">
        <CommandMenu />

        {mounted && (
            <div className="flex items-center gap-1 border-l border-border pl-4">
                <button onClick={() => setTheme("light")} className="p-2 rounded-md hover:bg-secondary text-muted-foreground hover:text-foreground" title="Light">
                    <Sun size={18} />
                </button>
                <button onClick={() => setTheme("dark")} className="p-2 rounded-md hover:bg-secondary text-muted-foreground hover:text-foreground" title="Dark">
                    <Moon size={18} />
                </button>
                <button onClick={() => setTheme("oceanic")} className="p-2 rounded-md hover:bg-secondary text-cyan-500" title="Oceanic">
                    <div className="w-4 h-4 rounded-full bg-cyan-500" />
                </button>
                <button onClick={() => setTheme("sunset")} className="p-2 rounded-md hover:bg-secondary text-orange-500" title="Sunset">
                    <div className="w-4 h-4 rounded-full bg-orange-500" />
                </button>
                <button onClick={() => setTheme("cyberpunk")} className="p-2 rounded-md hover:bg-secondary text-pink-500" title="Cyberpunk">
                    <Zap size={18} />
                </button>
                <button onClick={() => setTheme("high-contrast")} className="p-2 rounded-md hover:bg-secondary text-foreground" title="High Contrast">
                    <Eye size={18} />
                </button>
            </div>
        )}
      </div>
    </header>
  );
}
