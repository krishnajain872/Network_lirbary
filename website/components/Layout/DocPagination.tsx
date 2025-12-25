"use client";

import Link from "next/link";
import { ChevronLeft, ChevronRight } from "lucide-react";

export function DocPagination() {
  return (
    <div className="flex justify-between items-center mt-16 pt-8 border-t border-border">
      <Link
        href="#"
        className="group flex flex-col items-start gap-1 p-4 rounded-lg border border-border hover:border-primary/50 hover:bg-accent/50 transition-all"
      >
        <span className="flex items-center gap-1 text-xs text-muted-foreground group-hover:text-primary transition-colors">
          <ChevronLeft size={12} /> Previous
        </span>
        <span className="font-medium">Getting Started</span>
      </Link>

      <Link
        href="#"
        className="group flex flex-col items-end gap-1 p-4 rounded-lg border border-border hover:border-primary/50 hover:bg-accent/50 transition-all"
      >
        <span className="flex items-center gap-1 text-xs text-muted-foreground group-hover:text-primary transition-colors">
          Next <ChevronRight size={12} />
        </span>
        <span className="font-medium">Architecture</span>
      </Link>
    </div>
  );
}
