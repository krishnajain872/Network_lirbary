"use client";

import Link from "next/link";
import { motion } from "framer-motion";
import { ArrowRight, Box, Zap, Shield } from "lucide-react";

export default function Home() {
  return (
    <div className="flex flex-col items-center justify-center min-h-[calc(100vh-4rem)] p-4 text-center">
      <motion.div
        initial={{ opacity: 0, y: 20 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ duration: 0.8 }}
        className="max-w-3xl"
      >
        <h1 className="text-6xl font-extrabold mb-6 tracking-tight">
          Next-Gen <span className="text-transparent bg-clip-text bg-gradient-to-r from-blue-500 to-cyan-400">Network Library</span>
        </h1>
        <p className="text-xl text-gray-600 dark:text-gray-300 mb-8 leading-relaxed">
          High-performance C++20 networking with io_uring, QUIC support, and zero-copy architecture.
          Built for scalability. Documented with AI.
        </p>

        <div className="flex flex-wrap justify-center gap-4 mb-16">
          <Link
            href="/docs/getting-started/quick-start"
            className="px-8 py-3 rounded-full bg-blue-600 hover:bg-blue-700 text-white font-semibold transition-all hover:scale-105 flex items-center gap-2"
          >
            Get Started <ArrowRight size={20} />
          </Link>
          <Link
            href="/jules-journey"
            className="px-8 py-3 rounded-full bg-white dark:bg-gray-800 border border-gray-200 dark:border-gray-700 hover:border-blue-500 text-foreground font-semibold transition-all hover:scale-105"
          >
            View AI Journey
          </Link>
        </div>
      </motion.div>

      <motion.div
        initial={{ opacity: 0, y: 40 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ delay: 0.4, duration: 0.8 }}
        className="grid md:grid-cols-3 gap-8 max-w-5xl w-full"
      >
        <FeatureCard
          icon={<Zap className="text-yellow-500" size={32} />}
          title="Blazing Fast"
          description="Powered by io_uring and C++20 coroutines for maximum throughput."
        />
        <FeatureCard
          icon={<Box className="text-purple-500" size={32} />}
          title="Modern Protocols"
          description="Native support for HTTP/2, gRPC, and QUIC out of the box."
        />
        <FeatureCard
          icon={<Shield className="text-green-500" size={32} />}
          title="Secure by Default"
          description="Built-in DDoS protection, TLS 1.3, and circuit breakers."
        />
      </motion.div>
    </div>
  );
}

function FeatureCard({ icon, title, description }: { icon: React.ReactNode, title: string, description: string }) {
  return (
    <div className="p-6 rounded-2xl bg-white/50 dark:bg-black/20 backdrop-blur-sm border border-gray-200 dark:border-gray-800 hover:border-blue-500/50 transition-colors text-left">
      <div className="mb-4 p-3 bg-gray-100 dark:bg-gray-900 rounded-lg w-fit">
        {icon}
      </div>
      <h3 className="text-xl font-bold mb-2">{title}</h3>
      <p className="text-gray-600 dark:text-gray-400">{description}</p>
    </div>
  );
}
