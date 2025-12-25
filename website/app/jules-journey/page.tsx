"use client";

import { motion } from "framer-motion";
import { julesJourneyData } from "@/data/journey";
import { Calendar, CheckCircle2, Terminal, Clock, Sparkles, Zap, Bug } from "lucide-react";

export default function JulesJourneyPage() {
  return (
    <div className="max-w-6xl mx-auto py-12 px-4">
      {/* Hero Section */}
      <motion.div
        initial={{ opacity: 0, y: 20 }}
        animate={{ opacity: 1, y: 0 }}
        className="text-center mb-16 space-y-6"
      >
        <div className="inline-flex items-center gap-2 px-3 py-1 rounded-full bg-primary/10 text-primary text-sm font-medium">
          <Sparkles size={14} /> AI-Powered Development
        </div>
        <h1 className="text-5xl md:text-6xl font-extrabold tracking-tight bg-clip-text text-transparent bg-gradient-to-r from-primary via-purple-500 to-pink-500 pb-2">
          AI Development Journey
        </h1>
        <p className="text-xl text-muted-foreground max-w-2xl mx-auto">
          A transparent chronological log of how Google Jules accelerated the development of NetworkLib, from compilation fixes to this premium documentation site.
        </p>
      </motion.div>

      {/* Metrics Grid */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-4 mb-24">
        <MetricCard icon={<Clock />} label="Time Saved" value="45+ Hours" color="text-blue-500" />
        <MetricCard icon={<Terminal />} label="Prompts Executed" value="12" color="text-purple-500" />
        <MetricCard icon={<Zap />} label="Features Built" value="8" color="text-yellow-500" />
        <MetricCard icon={<Bug />} label="Bugs Fixed" value="15+" color="text-green-500" />
      </div>

      {/* Timeline */}
      <div className="relative border-l-2 border-border ml-4 md:ml-8 space-y-16">
        {julesJourneyData.map((item, index) => (
          <motion.div
            key={item.id}
            initial={{ opacity: 0, x: -20 }}
            whileInView={{ opacity: 1, x: 0 }}
            viewport={{ once: true }}
            transition={{ delay: index * 0.1 }}
            className="relative pl-8 md:pl-12"
          >
            {/* Timeline Dot */}
            <div className="absolute -left-[11px] top-0 w-6 h-6 rounded-full bg-background border-4 border-primary shadow-lg shadow-primary/20" />

            {/* Content Card */}
            <div className="group glass rounded-2xl p-1 shadow-lg transition-all hover:shadow-primary/10 hover:-translate-y-1">
              <div className="bg-card/50 backdrop-blur-xl rounded-xl p-6 md:p-8 border border-border/50">

                {/* Header */}
                <div className="flex flex-wrap items-center justify-between gap-4 mb-6">
                  <div className="flex items-center gap-3">
                    <span className="flex items-center justify-center w-10 h-10 rounded-lg bg-primary/10 text-primary">
                        {index === 0 ? <Bug size={20} /> : <Sparkles size={20} />}
                    </span>
                    <div>
                        <h2 className="text-2xl font-bold">{item.title}</h2>
                        <div className="flex items-center gap-2 text-sm text-muted-foreground">
                            <Calendar size={14} />
                            {item.date}
                        </div>
                    </div>
                  </div>
                  <div className="flex gap-2">
                    {item.tags.map((tag) => (
                      <span
                        key={tag}
                        className="px-3 py-1 text-xs font-medium rounded-full bg-secondary text-secondary-foreground border border-border"
                      >
                        {tag}
                      </span>
                    ))}
                  </div>
                </div>

                {/* Prompt Section */}
                <div className="bg-secondary/30 p-4 rounded-xl border border-border mb-6 group-hover:border-primary/20 transition-colors">
                  <div className="flex items-start gap-3">
                    <Terminal className="mt-1 text-primary shrink-0" size={18} />
                    <div className="w-full">
                      <span className="text-xs font-bold text-primary uppercase tracking-wider block mb-2">
                        Prompt
                      </span>
                      <p className="font-mono text-sm text-foreground/90 whitespace-pre-wrap">
                        {item.prompt}
                      </p>
                    </div>
                  </div>
                </div>

                {/* Problem vs Solution */}
                <div className="grid md:grid-cols-2 gap-6">
                  <div className="space-y-3">
                    <div className="flex items-center gap-2 text-sm font-bold text-red-500 uppercase tracking-wider">
                        <div className="w-2 h-2 rounded-full bg-red-500" /> The Problem
                    </div>
                    <p className="text-muted-foreground text-sm leading-relaxed border-l-2 border-red-500/20 pl-4">
                      {item.problem}
                    </p>
                  </div>

                  <div className="space-y-3">
                    <div className="flex items-center gap-2 text-sm font-bold text-green-500 uppercase tracking-wider">
                        <div className="w-2 h-2 rounded-full bg-green-500" /> The Solution
                    </div>
                    <div className="text-muted-foreground text-sm leading-relaxed border-l-2 border-green-500/20 pl-4 whitespace-pre-line">
                      {item.solution}
                    </div>
                  </div>
                </div>

              </div>
            </div>
          </motion.div>
        ))}
      </div>
    </div>
  );
}

function MetricCard({ icon, label, value, color }: { icon: React.ReactNode, label: string, value: string, color: string }) {
    return (
        <div className="glass p-6 rounded-2xl border border-border flex items-center gap-4">
            <div className={`p-3 rounded-xl bg-secondary/50 ${color}`}>
                {icon}
            </div>
            <div>
                <div className="text-2xl font-bold">{value}</div>
                <div className="text-sm text-muted-foreground">{label}</div>
            </div>
        </div>
    )
}
