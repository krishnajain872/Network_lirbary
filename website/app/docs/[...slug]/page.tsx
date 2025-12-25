import { MDXRemote } from "next-mdx-remote/rsc";
import { getDocBySlug } from "@/lib/docs";
import { notFound } from "next/navigation";
import { Breadcrumbs } from "@/components/Layout/Breadcrumbs";
import { TOC } from "@/components/Layout/TOC";
import { DocPagination } from "@/components/Layout/DocPagination";
import rehypeSlug from "rehype-slug";
import rehypeAutolinkHeadings from "rehype-autolink-headings";

export default async function DocPage({ params }: { params: Promise<{ slug: string[] }> }) {
  const { slug } = await params;
  const doc = getDocBySlug(slug);

  if (!doc) {
    notFound();
  }

  return (
    <div className="flex justify-center w-full px-4 md:px-8 py-8 gap-8">
        <article className="prose dark:prose-invert max-w-3xl w-full min-w-0">
            <Breadcrumbs />
            <div className="mb-8 border-b border-border pb-4">
                <h1 className="text-4xl font-bold mb-2">{doc.frontmatter.title}</h1>
                {doc.frontmatter.description && (
                <p className="text-xl text-muted-foreground">
                    {doc.frontmatter.description}
                </p>
                )}
            </div>

            <MDXRemote
                source={doc.content}
                options={{
                    mdxOptions: {
                        rehypePlugins: [
                            rehypeSlug,
                            [rehypeAutolinkHeadings, { behavior: "wrap" }]
                        ]
                    }
                }}
            />

            <DocPagination />
        </article>

        <TOC headings={doc.headings} />
    </div>
  );
}

export async function generateStaticParams() {
    return [];
}
