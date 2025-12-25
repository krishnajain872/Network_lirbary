"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { cn } from "@/lib/utils";
import { motion, AnimatePresence } from "framer-motion";
import {
  BookOpen,
  Cpu,
  Settings,
  Network,
  Code2,
  ChevronRight,
  ChevronDown,
  Menu,
  X
} from "lucide-react";
import { useState } from "react";
import { Button } from "./ui/button";
import { Card } from "@/components/ui/card";

const navItems = [
  {
    title: "Getting Started",
    href: "/getting-started",
    icon: BookOpen,
  },
  {
    title: "Architecture",
    href: "/architecture",
    icon: Cpu,
  },
  {
    title: "Configuration",
    href: "/configuration",
    icon: Settings,
  },
  {
    title: "Protocols",
    href: "/protocols",
    icon: Network,
  },
  {
    title: "Developer Guide",
    href: "/developer-guide",
    icon: Code2,
  },
  {
    title: "Use Cases",
    href: "/use-cases",
    icon: BookOpen,
  },
  {
    title: "Improvements",
    href: "/improvements",
    icon: Settings,
  },
  {
    title: "Pros & Cons",
    href: "/pros-cons",
    icon: Cpu,
  },
];

export function Sidebar({ className }: { className?: string }) {
  const pathname = usePathname();
  const [isOpen, setIsOpen] = useState(false); // Mobile state
  const [isCollapsed, setIsCollapsed] = useState(false); // Desktop state

  return (
    <>
      {/* Mobile Trigger */}
      <Button
        variant="ghost"
        size="icon"
        className="lg:hidden fixed top-3 left-4 z-50"
        onClick={() => setIsOpen(!isOpen)}
      >
        {isOpen ? <X /> : <Menu />}
      </Button>

      {/* Sidebar Container */}
      <motion.aside
        initial={false}
        animate={{
          width: isCollapsed ? "80px" : "280px",
          x: isOpen ? 0 : "-100%", // Slide in on mobile
        }}
        // Reset x on desktop (lg breakpoint is 1024px)
        style={{ x: 0 }} // We will handle mobile responsiveness with CSS classes mostly
        className={cn(
          "fixed top-0 left-0 z-40 h-screen border-r bg-card/50 backdrop-blur-xl transition-all duration-300 ease-in-out",
          "lg:translate-x-0", // Always visible on desktop
          // Mobile specific override handled by the animate prop if we were doing pure JS,
          // but mixing Framer Motion with Tailwind breakpoints can be tricky.
          // Let's use a conditional class for mobile visibility
          !isOpen && "max-lg:-translate-x-full",
          className
        )}
      >
        <div className="flex h-16 items-center justify-between px-6 border-b">
          {!isCollapsed && (
            <Link href="/" className="flex items-center gap-2 font-bold text-xl tracking-tight">
               <div className="w-8 h-8 rounded-lg bg-primary flex items-center justify-center text-primary-foreground">
                  NL
               </div>
               <span>NetworkLib</span>
            </Link>
          )}
          {isCollapsed && (
             <Link href="/" className="mx-auto">
               <div className="w-8 h-8 rounded-lg bg-primary flex items-center justify-center text-primary-foreground">
                  NL
               </div>
             </Link>
          )}

          <Button
             variant="ghost"
             size="icon"
             className="hidden lg:flex"
             onClick={() => setIsCollapsed(!isCollapsed)}
          >
             {isCollapsed ? <ChevronRight size={16}/> : <ChevronDown size={16} className="rotate-90"/>}
          </Button>
        </div>

        <nav className="p-4 space-y-2">
          {navItems.map((item) => {
            const isActive = pathname.startsWith(item.href);
            return (
              <Link
                key={item.href}
                href={item.href}
                className={cn(
                  "flex items-center gap-3 rounded-lg px-3 py-2 text-sm font-medium transition-all hover:bg-accent hover:text-accent-foreground",
                  isActive ? "bg-accent text-accent-foreground shadow-sm" : "text-muted-foreground",
                  isCollapsed && "justify-center px-2"
                )}
              >
                <item.icon className="h-4 w-4" />
                {!isCollapsed && <span>{item.title}</span>}
                {isActive && !isCollapsed && (
                  <motion.div
                    layoutId="sidebar-active"
                    className="ml-auto w-1.5 h-1.5 rounded-full bg-primary"
                  />
                )}
              </Link>
            );
          })}
        </nav>

        {!isCollapsed && (
           <div className="absolute bottom-4 left-0 right-0 px-4">
              <Card className="bg-primary/5 border-none">
                 <div className="p-3 text-xs text-muted-foreground">
                    <p className="font-semibold text-foreground mb-1">Latest Version</p>
                    <p>v1.0.0 (Stable)</p>
                 </div>
              </Card>
           </div>
        )}
      </motion.aside>
    </>
  );
}
