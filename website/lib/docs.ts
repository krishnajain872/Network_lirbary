import fs from "fs";
import path from "path";
import matter from "gray-matter";

const contentDirectory = path.join(process.cwd(), "content");

export interface DocPost {
  slug: string[];
  frontmatter: {
    title: string;
    description?: string;
    [key: string]: any;
  };
  content: string;
}

export function getAllDocs(): DocPost[] {
  // Recursive function to get all md/mdx files
  const getFiles = (dir: string): string[] => {
    const files = fs.readdirSync(dir);
    let allFiles: string[] = [];

    files.forEach((file) => {
      const fullPath = path.join(dir, file);
      if (fs.statSync(fullPath).isDirectory()) {
        allFiles = [...allFiles, ...getFiles(fullPath)];
      } else if (file.endsWith(".md") || file.endsWith(".mdx")) {
        allFiles.push(fullPath);
      }
    });
    return allFiles;
  };

  const files = getFiles(contentDirectory);

  return files.map((fullPath) => {
    const relativePath = path.relative(contentDirectory, fullPath);
    const slug = relativePath.replace(/\.mdx?$/, "").split(path.sep);

    const fileContents = fs.readFileSync(fullPath, "utf8");
    const { data, content } = matter(fileContents);

    return {
      slug,
      frontmatter: data as DocPost["frontmatter"],
      content,
    };
  });
}

export function getDocBySlug(slug: string[]) {
  const realSlug = slug.join(path.sep);
  const fullPath = path.join(contentDirectory, `${realSlug}.mdx`);

  if (!fs.existsSync(fullPath)) {
    return null;
  }

  const fileContents = fs.readFileSync(fullPath, "utf8");
  const { data, content } = matter(fileContents);

  return {
    slug,
    frontmatter: data,
    content,
  };
}
