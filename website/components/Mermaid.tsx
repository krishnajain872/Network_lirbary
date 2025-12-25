"use client";

import { useEffect, useRef } from "react";
import mermaid from "mermaid";
import { useTheme } from "next-themes";

export function Mermaid({ chart }: { chart: string }) {
  const ref = useRef<HTMLDivElement>(null);
  const { theme } = useTheme();

  useEffect(() => {
    mermaid.initialize({
      startOnLoad: true,
      theme: theme === 'light' ? 'default' : 'dark',
      securityLevel: 'loose',
      fontFamily: 'var(--font-geist-sans)',
    });

    if (ref.current) {
      mermaid.contentLoaded();
    }
  }, [theme]);

  useEffect(() => {
    if (ref.current) {
      ref.current.removeAttribute('data-processed');
      mermaid.run({
         nodes: [ref.current]
      });
    }
  }, [chart, theme]);

  return (
    <div className="mermaid flex justify-center p-4 bg-background/50 rounded-lg" ref={ref}>
      {chart}
    </div>
  );
}
