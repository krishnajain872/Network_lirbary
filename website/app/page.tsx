"use client";

import { PageTransition } from "@/components/PageTransition";
import { ParticleNetwork } from "@/components/visuals/ParticleNetwork";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import Link from "next/link";
import { ArrowRight, Zap, Shield, Globe, Cpu, Server, Activity } from "lucide-react";

export default function Home() {
  return (
    <>
      <ParticleNetwork />
      <PageTransition>
        <section className="flex flex-col items-center justify-center py-20 text-center space-y-8">
          <Badge variant="secondary" className="px-4 py-2 text-sm animate-fade-in">
            Built with C++20 & io_uring
          </Badge>

          <h1 className="text-5xl lg:text-7xl font-bold tracking-tight bg-gradient-to-r from-primary to-ring bg-clip-text text-transparent pb-2">
            NetworkLib
          </h1>

          <p className="text-xl text-muted-foreground max-w-2xl">
             A configuration-driven, zero-code protocol-switching network library designed for
             <strong> 10 Billion+ Concurrent Connections</strong>.
          </p>

          <div className="flex gap-4">
             <Button size="lg" asChild>
                <Link href="/getting-started">
                   Get Started <ArrowRight className="ml-2 h-4 w-4" />
                </Link>
             </Button>
             <Button size="lg" variant="outline" asChild>
                <Link href="/architecture">View Architecture</Link>
             </Button>
          </div>
        </section>

        {/* Stats Section */}
        <section className="grid grid-cols-2 lg:grid-cols-4 gap-4 py-10">
           {[
              { label: "Connections", value: "10B+", icon: Globe },
              { label: "Latency P99", value: "< 500μs", icon: Zap },
              { label: "Protocols", value: "7+", icon: NetworkIcon },
              { label: "Memory/Conn", value: "< 512B", icon: Cpu },
           ].map((stat, i) => (
              <Card key={i} className="bg-card/50 backdrop-blur-sm border-primary/10">
                 <CardContent className="flex flex-col items-center justify-center p-6 text-center">
                    <stat.icon className="h-8 w-8 mb-4 text-primary" />
                    <div className="text-3xl font-bold">{stat.value}</div>
                    <div className="text-sm text-muted-foreground">{stat.label}</div>
                 </CardContent>
              </Card>
           ))}
        </section>

        {/* Features Grid */}
        <section className="py-20">
           <h2 className="text-3xl font-bold mb-10 text-center">Why NetworkLib?</h2>
           <div className="grid md:grid-cols-3 gap-6">
              <FeatureCard
                 title="Zero-Code Protocol Switching"
                 description="Switch between TCP, HTTP, gRPC, and WebSocket just by changing a YAML config file. No recompilation needed."
                 icon={Server}
              />
              <FeatureCard
                 title="Extreme Performance"
                 description="Built on Linux io_uring and epoll with thread-per-core architecture to maximize hardware utilization."
                 icon={Activity}
              />
              <FeatureCard
                 title="Enterprise Security"
                 description="Native TLS 1.3 support, DDoS protection stubs, and comprehensive audit logging out of the box."
                 icon={Shield}
              />
           </div>
        </section>
      </PageTransition>
    </>
  );
}

function FeatureCard({ title, description, icon: Icon }: any) {
   return (
      <Card className="hover:border-primary/50 transition-colors">
         <CardHeader>
            <Icon className="h-10 w-10 text-primary mb-2" />
            <CardTitle>{title}</CardTitle>
         </CardHeader>
         <CardContent>
            <p className="text-muted-foreground">{description}</p>
         </CardContent>
      </Card>
   )
}

function NetworkIcon(props: any) {
   return (
      <svg
      {...props}
      xmlns="http://www.w3.org/2000/svg"
      width="24"
      height="24"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      strokeWidth="2"
      strokeLinecap="round"
      strokeLinejoin="round"
    >
      <rect x="16" y="16" width="6" height="6" rx="1" />
      <rect x="2" y="16" width="6" height="6" rx="1" />
      <rect x="9" y="2" width="6" height="6" rx="1" />
      <path d="M5 16v-3a1 1 0 0 1 1-1h12a1 1 0 0 1 1 1v3" />
      <path d="M12 12V8" />
    </svg>
   )
}
