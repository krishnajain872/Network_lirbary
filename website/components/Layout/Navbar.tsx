"use client";

import Link from "next/link";
import { Search, Sun, Moon, Monitor, Menu } from "lucide-react";
import { useTheme } from "next-themes";
import { useState, useEffect } from "react";

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
    <header className="fixed top-0 left-0 right-0 z-50 h-16 bg-background/80 backdrop-blur-md border-b border-gray-200 dark:border-gray-800 flex items-center justify-between px-4 md:px-8">
      <div className="flex items-center gap-4">
        <button
          onClick={onMenuClick}
          className="md:hidden p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded-md"
        >
          <Menu size={24} />
        </button>
        <Link href="/" className="text-xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-purple-600">
          NetworkLib Docs
        </Link>
      </div>

      <div className="flex items-center gap-4">
        <div className="hidden md:flex items-center relative">
          <Search className="absolute left-3 top-1/2 -translate-y-1/2 text-gray-400" size={16} />
          <input
            type="text"
            placeholder="Search documentation... (Cmd+K)"
            className="pl-10 pr-4 py-2 rounded-full bg-gray-100 dark:bg-gray-800 border-none focus:ring-2 focus:ring-blue-500 text-sm w-64"
          />
        </div>

        {mounted && (
            <div className="flex items-center gap-2 border-l border-gray-200 dark:border-gray-700 pl-4">
                <button
                    onClick={() => setTheme("light")}
                    className={`p-2 rounded-md ${theme === 'light' ? 'bg-blue-100 text-blue-600' : 'hover:bg-gray-100'}`}
                    title="Light Mode"
                >
                    <Sun size={20} />
                </button>
                <button
                    onClick={() => setTheme("dark")}
                    className={`p-2 rounded-md ${theme === 'dark' ? 'bg-blue-900/50 text-blue-400' : 'hover:bg-gray-800'}`}
                    title="Dark Mode"
                >
                    <Moon size={20} />
                </button>
                <button
                    onClick={() => setTheme("oceanic")}
                    className={`p-2 rounded-md ${theme === 'oceanic' ? 'bg-cyan-100 text-cyan-600' : 'hover:bg-gray-100'}`}
                    title="Oceanic Mode"
                >
                    <span className="block w-5 h-5 rounded-full bg-cyan-500"></span>
                </button>
                 <button
                    onClick={() => setTheme("sunset")}
                    className={`p-2 rounded-md ${theme === 'sunset' ? 'bg-orange-100 text-orange-600' : 'hover:bg-gray-100'}`}
                    title="Sunset Mode"
                >
                    <span className="block w-5 h-5 rounded-full bg-orange-500"></span>
                </button>
            </div>
        )}
      </div>
    </header>
  );
}
