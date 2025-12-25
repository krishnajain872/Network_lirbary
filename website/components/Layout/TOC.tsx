"use client";

import * as React from "react";
import Link from "next/link";
import { cn } from "@/lib/utils";

interface TOCProps {
  headings: { id: string; text: string; level: number }[];
}

export function TOC({ headings }: TOCProps) {
  const [activeId, setActiveId] = React.useState<string>("");

  React.useEffect(() => {
    const observer = new IntersectionObserver(
      (entries) => {
        entries.forEach((entry) => {
          if (entry.isIntersecting) {
            setActiveId(entry.target.id);
          }
        });
      },
      { rootMargin: "0% 0% -80% 0%" }
    );

    headings.forEach((heading) => {
      const element = document.getElementById(heading.id);
      if (element) observer.observe(element);
    });

    return () => observer.disconnect();
  }, [headings]);

  if (headings.length === 0) return null;

  return (
    <div className="hidden xl:block w-64 shrink-0 order-last">
      <div className="sticky top-24 pl-4 border-l border-border">
        <h4 className="mb-4 text-sm font-semibold text-foreground">On this page</h4>
        <nav className="flex flex-col gap-2">
          {headings.map((heading) => (
            <Link
              key={heading.id}
              href={`#${heading.id}`}
              className={cn(
                "text-sm transition-colors hover:text-primary line-clamp-1",
                heading.level === 3 && "pl-4",
                activeId === heading.id
                  ? "text-primary font-medium"
                  : "text-muted-foreground"
              )}
            >
              {heading.text}
            </Link>
          ))}
        </nav>
      </div>
    </div>
  );
}
