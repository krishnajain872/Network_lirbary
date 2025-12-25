import { MDXRemote } from "next-mdx-remote/rsc";
import { getDocBySlug } from "@/lib/docs";
import { notFound } from "next/navigation";

export default async function DocPage({ params }: { params: Promise<{ slug: string[] }> }) {
  const { slug } = await params;
  const doc = getDocBySlug(slug);

  if (!doc) {
    notFound();
  }

  return (
    <div className="prose dark:prose-invert max-w-4xl mx-auto py-10 px-4">
      <div className="mb-8 border-b border-gray-200 dark:border-gray-800 pb-4">
        <h1 className="text-4xl font-bold mb-2">{doc.frontmatter.title}</h1>
        {doc.frontmatter.description && (
          <p className="text-xl text-gray-500 dark:text-gray-400">
            {doc.frontmatter.description}
          </p>
        )}
      </div>

      <MDXRemote source={doc.content} />
    </div>
  );
}

export async function generateStaticParams() {
    return [];
}
