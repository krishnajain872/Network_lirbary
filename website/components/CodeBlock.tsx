"use client";

import { useState } from "react";
import { Check, Copy } from "lucide-react";
import { Button } from "@/components/ui/button";

interface CodeBlockProps {
  language: string;
  code: string;
  filename?: string;
}

export function CodeBlock({ language, code, filename }: CodeBlockProps) {
  const [copied, setCopied] = useState(false);

  const copyToClipboard = async () => {
    await navigator.clipboard.writeText(code);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  return (
    <div className="relative group rounded-lg border bg-muted/40 my-4 overflow-hidden">
      {filename && (
        <div className="flex items-center justify-between px-4 py-2 border-b bg-muted/50">
          <span className="text-xs font-medium text-muted-foreground">{filename}</span>
          <span className="text-xs text-muted-foreground uppercase">{language}</span>
        </div>
      )}

      <div className="relative">
        <pre className="p-4 overflow-x-auto text-sm font-mono">
          <code className={`language-${language}`}>{code}</code>
        </pre>

        <Button
          size="icon"
          variant="ghost"
          className="absolute top-2 right-2 h-8 w-8 opacity-0 group-hover:opacity-100 transition-opacity"
          onClick={copyToClipboard}
        >
          {copied ? <Check className="h-4 w-4" /> : <Copy className="h-4 w-4" />}
        </Button>
      </div>
    </div>
  );
}
