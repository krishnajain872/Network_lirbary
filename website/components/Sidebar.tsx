import Link from "next/link";
import { BookOpen, Cpu, Settings, Network, Code2, Home, Activity, CheckCircle, Lightbulb } from "lucide-react";

const navItems = [
  { title: "Home", href: "/", icon: Home },
  { title: "Getting Started", href: "/getting-started", icon: BookOpen },
  { title: "Architecture", href: "/architecture", icon: Cpu },
  { title: "Configuration", href: "/configuration", icon: Settings },
  { title: "Protocols", href: "/protocols", icon: Network },
  { title: "Developer Guide", href: "/developer-guide", icon: Code2 },
  { title: "Use Cases", href: "/use-cases", icon: Activity },
  { title: "Improvements", href: "/improvements", icon: Lightbulb },
  { title: "Pros & Cons", href: "/pros-cons", icon: CheckCircle },
];

export function Sidebar() {
  return (
    <aside className="fixed top-0 left-0 z-40 h-screen w-[280px] border-r bg-card/50 backdrop-blur-xl hidden lg:block">
      <div className="flex h-16 items-center px-6 border-b">
        <span className="font-bold text-xl">NetworkLib</span>
      </div>
      <nav className="p-4 space-y-2 overflow-y-auto h-[calc(100vh-64px)]">
        {navItems.map((item) => (
          <Link
            key={item.href}
            href={item.href}
            className="flex items-center gap-3 rounded-lg px-3 py-2 text-sm font-medium hover:bg-muted text-muted-foreground hover:text-foreground transition-colors"
          >
            <item.icon className="h-4 w-4" />
            <span>{item.title}</span>
          </Link>
        ))}
      </nav>
    </aside>
  );
}
