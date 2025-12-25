import { ParticleNetwork } from "@/components/visuals/ParticleNetwork";

export default function Home() {
  return (
    <>
      <ParticleNetwork />
      <div className="relative p-20 flex flex-col items-center justify-center min-h-[calc(100vh-64px)] text-center space-y-8">
        <div className="inline-block px-4 py-1.5 mb-4 text-sm font-semibold tracking-wider text-blue-400 uppercase bg-blue-900/30 rounded-full border border-blue-800">
          Built with C++20 & io_uring
        </div>

        <h1 className="text-6xl md:text-8xl font-bold tracking-tighter bg-gradient-to-br from-white to-slate-500 bg-clip-text text-transparent">
          NetworkLib
        </h1>

        <p className="max-w-2xl text-xl text-slate-400">
          A high-performance, configuration-driven network library designed for
          <span className="text-blue-400 font-bold"> 10 Billion+ Concurrent Connections</span>.
        </p>

        <div className="flex gap-4 pt-4">
          <a href="/getting-started" className="px-8 py-3 font-medium text-white bg-blue-600 rounded-lg hover:bg-blue-700 transition-colors">
            Get Started
          </a>
          <a href="/architecture" className="px-8 py-3 font-medium text-slate-300 border border-slate-700 rounded-lg hover:bg-slate-800 transition-colors">
            Architecture
          </a>
        </div>
      </div>
    </>
  );
}
