"use client";

import Link from "next/link";
import { ChevronRight, Home } from "lucide-react";
import { usePathname } from "next/navigation";

export function Breadcrumbs() {
  const pathname = usePathname();
  const segments = pathname.split("/").filter(Boolean);

  if (segments.length === 0) return null;

  return (
    <nav className="flex items-center space-x-1 text-sm text-muted-foreground mb-6">
      <Link href="/" className="hover:text-primary transition-colors">
        <Home size={14} />
      </Link>
      {segments.map((segment, index) => {
        const path = `/${segments.slice(0, index + 1).join("/")}`;
        const isLast = index === segments.length - 1;
        const title = segment.replace(/-/g, " ").replace(/\b\w/g, (l) => l.toUpperCase());

        return (
          <div key={path} className="flex items-center space-x-1">
            <ChevronRight size={14} className="text-muted-foreground/50" />
            {isLast ? (
              <span className="text-foreground font-medium">{title}</span>
            ) : (
              <Link href={path} className="hover:text-primary transition-colors">
                {title}
              </Link>
            )}
          </div>
        );
      })}
    </nav>
  );
}
