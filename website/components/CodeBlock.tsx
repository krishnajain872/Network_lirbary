import { codeToHtml } from "shiki";

export async function CodeBlock({ code, lang, filename }: { code: string; lang: string; filename?: string }) {
  const html = await codeToHtml(code, {
    lang,
    theme: "github-dark",
  });

  return (
    <div className="my-4 rounded-lg overflow-hidden border border-slate-800 bg-[#0d1117]">
      {filename && (
        <div className="px-4 py-2 bg-slate-900 border-b border-slate-800 text-xs text-slate-400 font-mono">
          {filename}
        </div>
      )}
      <div
        className="p-4 overflow-x-auto text-sm font-mono [&>pre]:!bg-transparent"
        dangerouslySetInnerHTML={{ __html: html }}
      />
    </div>
  );
}
