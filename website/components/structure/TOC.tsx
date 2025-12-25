"use client";

import { useEffect, useState } from "react";
import { cn } from "@/lib/utils";

interface TOCItem {
  id: string;
  text: string;
  level: number;
}

export function TOC() {
  const [headings, setHeadings] = useState<TOCItem[]>([]);
  const [activeId, setActiveId] = useState<string>("");

  useEffect(() => {
    // Client-side: Find all h2 and h3 elements inside the main content
    // We assume main content is in a `main` tag or specific container.
    // Let's target `main` for now.
    const elements = Array.from(document.querySelectorAll("main h2, main h3"));
    const items = elements.map((elem) => {
      if (!elem.id) {
        // Generate an ID if missing
        elem.id = elem.textContent?.toLowerCase().replace(/\s+/g, "-").replace(/[^\w-]/g, "") || "";
      }
      return {
        id: elem.id,
        text: elem.textContent || "",
        level: Number(elem.tagName.substring(1)),
      };
    });
    setHeadings(items);

    const observer = new IntersectionObserver(
      (entries) => {
        entries.forEach((entry) => {
          if (entry.isIntersecting) {
            setActiveId(entry.target.id);
          }
        });
      },
      { rootMargin: "0px 0px -80% 0px" }
    );

    elements.forEach((elem) => observer.observe(elem));

    return () => observer.disconnect();
  }, []);

  if (headings.length === 0) return null;

  return (
    <div className="hidden xl:block fixed top-24 right-8 w-64">
       <h4 className="font-semibold mb-4 text-sm text-muted-foreground uppercase tracking-wider">On This Page</h4>
       <ul className="space-y-2 border-l border-border pl-4">
          {headings.map((item) => (
             <li key={item.id} className={cn("text-sm transition-colors", item.level === 3 && "pl-4")}>
                <a
                   href={`#${item.id}`}
                   className={cn(
                      "block hover:text-foreground",
                      activeId === item.id ? "text-primary font-medium" : "text-muted-foreground"
                   )}
                   onClick={(e) => {
                      e.preventDefault();
                      document.getElementById(item.id)?.scrollIntoView({ behavior: "smooth" });
                      setActiveId(item.id);
                   }}
                >
                   {item.text}
                </a>
             </li>
          ))}
       </ul>
    </div>
  );
}
