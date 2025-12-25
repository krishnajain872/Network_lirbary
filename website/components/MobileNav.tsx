"use client";

import { Drawer } from "vaul";
import { Menu } from "lucide-react";
import { Button } from "@/components/ui/button";
import { useState } from "react";
import Link from "next/link";
import { usePathname } from "next/navigation";
import { cn } from "@/lib/utils";
import { BookOpen, Cpu, Settings, Network, Code2, Home } from "lucide-react";

export function MobileNav() {
  const [open, setOpen] = useState(false);
  const pathname = usePathname();

  const navItems = [
    { title: "Home", href: "/", icon: Home },
    { title: "Getting Started", href: "/getting-started", icon: BookOpen },
    { title: "Architecture", href: "/architecture", icon: Cpu },
    { title: "Configuration", href: "/configuration", icon: Settings },
    { title: "Protocols", href: "/protocols", icon: Network },
    { title: "Developer Guide", href: "/developer-guide", icon: Code2 },
    { title: "Use Cases", href: "/use-cases", icon: BookOpen },
    { title: "Improvements", href: "/improvements", icon: Settings },
    { title: "Pros & Cons", href: "/pros-cons", icon: Cpu },
  ];

  return (
    <Drawer.Root open={open} onOpenChange={setOpen}>
      <Drawer.Trigger asChild>
        <Button variant="ghost" size="icon" className="lg:hidden">
          <Menu className="h-6 w-6" />
        </Button>
      </Drawer.Trigger>
      <Drawer.Portal>
        <Drawer.Overlay className="fixed inset-0 bg-black/40 z-50" />
        <Drawer.Content className="bg-background flex flex-col rounded-t-[10px] h-[90%] mt-24 fixed bottom-0 left-0 right-0 z-50 focus:outline-none">
          <div className="p-4 bg-muted/40 rounded-t-[10px] flex-1 overflow-y-auto">
            <div className="mx-auto w-12 h-1.5 flex-shrink-0 rounded-full bg-muted mb-8" />
            <div className="flex flex-col gap-2">
              {navItems.map((item) => (
                <Link
                  key={item.href}
                  href={item.href}
                  onClick={() => setOpen(false)}
                  className={cn(
                    "flex items-center gap-4 px-4 py-3 rounded-lg text-lg font-medium transition-colors",
                    pathname === item.href
                      ? "bg-primary/10 text-primary"
                      : "hover:bg-muted"
                  )}
                >
                  <item.icon className="h-5 w-5" />
                  {item.title}
                </Link>
              ))}
            </div>
          </div>
        </Drawer.Content>
      </Drawer.Portal>
    </Drawer.Root>
  );
}
