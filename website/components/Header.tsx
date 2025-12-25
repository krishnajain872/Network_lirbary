"use client";

import { useTheme } from "next-themes";
import { Button } from "./ui/button";
import { MobileNav } from "@/components/MobileNav";
import {
   Moon,
   Sun,
   Monitor,
   Waves,
   Sunset,
   Zap,
   Search,
   Command
} from "lucide-react";
import {
   DropdownMenu,
   DropdownMenuContent,
   DropdownMenuItem,
   DropdownMenuTrigger
} from "@radix-ui/react-dropdown-menu";
import { cn } from "@/lib/utils";
import { useEffect, useState } from "react";
// Note: We'll implement the actual Search Dialog later, this is just the trigger.

export function Header() {
  const { setTheme, theme } = useTheme();
  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
  }, []);

  if (!mounted) return null;

  const themes = [
    { name: 'dark', icon: Moon, label: 'Dark' },
    { name: 'light', icon: Sun, label: 'Light' },
    { name: 'high-contrast', icon: Monitor, label: 'High Contrast' },
    { name: 'oceanic', icon: Waves, label: 'Oceanic' },
    { name: 'sunset', icon: Sunset, label: 'Sunset' },
    { name: 'cyberpunk', icon: Zap, label: 'Cyberpunk' },
  ];

  const currentTheme = themes.find(t => t.name === theme) || themes[0];
  const Icon = currentTheme.icon;

  return (
    <header className="sticky top-0 z-30 flex h-16 items-center gap-4 border-b bg-background/80 px-4 lg:px-6 backdrop-blur-xl">
       <MobileNav />
       <div className="flex-1">
          <Button
            variant="outline"
            className="w-full max-w-sm justify-start text-muted-foreground bg-muted/50 hover:bg-muted"
            onClick={() => document.dispatchEvent(new KeyboardEvent('keydown', { key: 'k', metaKey: true }))}
          >
             <Search className="mr-2 h-4 w-4" />
             <span>Search documentation...</span>
             <kbd className="pointer-events-none ml-auto inline-flex h-5 select-none items-center gap-1 rounded border bg-background px-1.5 font-mono text-[10px] font-medium text-muted-foreground opacity-100">
               <span className="text-xs">⌘</span>K
             </kbd>
          </Button>
       </div>

       <div className="flex items-center gap-2">
          {/* Theme Toggle - Simple cycling for now or dropdown if we had shadcn dropdown installed */}
          {/* Since we didn't install dropdown-menu, let's just cycle or use a simple select styled as a button */}
          <div className="flex gap-1 p-1 bg-muted rounded-lg">
             {themes.map((t) => (
                <button
                   key={t.name}
                   onClick={() => setTheme(t.name)}
                   className={cn(
                      "p-2 rounded-md transition-all hover:bg-background hover:shadow-sm",
                      theme === t.name ? "bg-background shadow-sm text-foreground" : "text-muted-foreground"
                   )}
                   title={t.label}
                >
                   <t.icon className="h-4 w-4" />
                </button>
             ))}
          </div>

          <Button asChild variant="default" size="sm" className="ml-2">
             <a href="https://github.com/your-repo/network-library" target="_blank" rel="noopener noreferrer">
                GitHub
             </a>
          </Button>
       </div>
    </header>
  );
}
