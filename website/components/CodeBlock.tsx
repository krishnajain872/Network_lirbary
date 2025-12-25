import { codeToHtml } from "shiki";
import { CopyButton } from "./CopyButton";

interface CodeBlockProps {
  language: string;
  code: string;
  filename?: string;
}

export async function CodeBlock({ language, code, filename }: CodeBlockProps) {
  const html = await codeToHtml(code, {
    lang: language,
    theme: "github-dark", // Premium dark theme matching our aesthetic
  });

  return (
    <div className="relative group rounded-lg border bg-[#0d1117] my-4 overflow-hidden shadow-sm">
      {filename && (
        <div className="flex items-center justify-between px-4 py-2 border-b border-white/10 bg-white/5">
          <span className="text-xs font-medium text-gray-400">{filename}</span>
          <span className="text-xs text-gray-500 uppercase">{language}</span>
        </div>
      )}

      <div className="relative">
        <div
           className="p-4 overflow-x-auto text-sm font-mono [&>pre]:!bg-transparent [&>pre]:!m-0"
           dangerouslySetInnerHTML={{ __html: html }}
        />
        <CopyButton text={code} />
      </div>
    </div>
  );
}
