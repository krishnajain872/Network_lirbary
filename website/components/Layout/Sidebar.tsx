"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { ChevronDown, ChevronRight, Menu } from "lucide-react";
import { useState } from "react";
import { motion, AnimatePresence } from "framer-motion";
import { cn } from "@/lib/utils";

interface SidebarProps {
  isOpen: boolean;
  setIsOpen: (open: boolean) => void;
}

const navItems = [
  {
    title: "Getting Started",
    path: "/docs/getting-started",
    items: [
      { title: "Quick Start", path: "/docs/getting-started/quick-start" },
      { title: "Installation", path: "/docs/getting-started/installation" },
    ],
  },
  {
    title: "Architecture",
    path: "/docs/architecture",
    items: [
      { title: "System Design", path: "/docs/architecture/system-design" },
      { title: "Data Flow", path: "/docs/architecture/data-flow" },
    ],
  },
  {
    title: "Configuration",
    path: "/docs/configuration",
    items: [
      { title: "YAML Guide", path: "/docs/configuration/yaml-guide" },
      { title: "Environment Vars", path: "/docs/configuration/env-vars" },
    ],
  },
  {
    title: "Jules Journey",
    path: "/jules-journey",
    items: [],
  },
];

export function Sidebar({ isOpen, setIsOpen }: SidebarProps) {
  return (
    <>
      {/* Mobile Overlay */}
      <AnimatePresence>
        {isOpen && (
          <motion.div
            initial={{ opacity: 0 }}
            animate={{ opacity: 0.5 }}
            exit={{ opacity: 0 }}
            onClick={() => setIsOpen(false)}
            className="fixed inset-0 bg-black z-40 md:hidden"
          />
        )}
      </AnimatePresence>

      {/* Sidebar Container */}
      <motion.aside
        className={cn(
          "fixed top-0 left-0 bottom-0 z-50 w-64 bg-background border-r border-gray-200 dark:border-gray-800 pt-16 pb-4 overflow-y-auto transform md:translate-x-0 transition-transform duration-200 ease-in-out",
          isOpen ? "translate-x-0" : "-translate-x-full"
        )}
      >
        <nav className="px-4 space-y-4">
          {navItems.map((section) => (
            <CollapsibleSection key={section.title} section={section} />
          ))}
        </nav>
      </motion.aside>
    </>
  );
}

function CollapsibleSection({ section }: { section: any }) {
  const [isOpen, setIsOpen] = useState(true);
  const pathname = usePathname();

  const isActive = pathname?.startsWith(section.path);

  return (
    <div className="space-y-1">
      {section.items.length > 0 ? (
        <button
          onClick={() => setIsOpen(!isOpen)}
          className="flex items-center justify-between w-full px-2 py-1.5 text-sm font-semibold text-foreground hover:bg-gray-100 dark:hover:bg-gray-800 rounded-md transition-colors"
        >
          <span>{section.title}</span>
          {isOpen ? <ChevronDown size={16} /> : <ChevronRight size={16} />}
        </button>
      ) : (
        <Link
          href={section.path}
           className={cn(
            "block px-2 py-1.5 text-sm font-semibold rounded-md transition-colors hover:bg-gray-100 dark:hover:bg-gray-800",
            isActive ? "text-blue-600 bg-blue-50 dark:bg-blue-900/20" : "text-foreground"
          )}
        >
          {section.title}
        </Link>
      )}

      <AnimatePresence>
        {isOpen && section.items.length > 0 && (
          <motion.div
            initial={{ height: 0, opacity: 0 }}
            animate={{ height: "auto", opacity: 1 }}
            exit={{ height: 0, opacity: 0 }}
            className="overflow-hidden ml-4 space-y-1 border-l border-gray-200 dark:border-gray-700 pl-4"
          >
            {section.items.map((item: any) => (
              <Link
                key={item.path}
                href={item.path}
                className={cn(
                  "block py-1 text-sm transition-colors hover:text-blue-500",
                  pathname === item.path
                    ? "text-blue-600 font-medium"
                    : "text-gray-600 dark:text-gray-400"
                )}
              >
                {item.title}
              </Link>
            ))}
          </motion.div>
        )}
      </AnimatePresence>
    </div>
  );
}
