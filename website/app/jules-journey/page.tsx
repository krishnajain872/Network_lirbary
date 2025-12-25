"use client";

import { motion } from "framer-motion";
import { julesJourneyData } from "@/data/journey";
import { Calendar, CheckCircle2, Terminal } from "lucide-react";

export default function JulesJourneyPage() {
  return (
    <div className="max-w-4xl mx-auto py-12 px-4">
      <motion.div
        initial={{ opacity: 0, y: 20 }}
        animate={{ opacity: 1, y: 0 }}
        className="text-center mb-16"
      >
        <h1 className="text-4xl font-bold mb-4 bg-clip-text text-transparent bg-gradient-to-r from-blue-600 to-purple-600">
          AI Development Journey
        </h1>
        <p className="text-xl text-gray-600 dark:text-gray-300">
          A chronological log of how Google Jules assisted in building NetworkLib.
        </p>
      </motion.div>

      <div className="relative border-l-2 border-blue-200 dark:border-blue-900 ml-4 md:ml-8 space-y-12">
        {julesJourneyData.map((item, index) => (
          <motion.div
            key={item.id}
            initial={{ opacity: 0, x: -20 }}
            whileInView={{ opacity: 1, x: 0 }}
            viewport={{ once: true }}
            transition={{ delay: index * 0.1 }}
            className="relative pl-8 md:pl-12"
          >
            {/* Dot */}
            <div className="absolute -left-[9px] top-0 w-4 h-4 rounded-full bg-blue-500 border-4 border-background" />

            {/* Content Card */}
            <div className="bg-white dark:bg-gray-900/50 backdrop-blur-sm rounded-xl p-6 shadow-lg border border-gray-100 dark:border-gray-800 hover:border-blue-500/30 transition-colors">
              <div className="flex flex-wrap items-center justify-between gap-4 mb-4">
                <div className="flex items-center gap-2 text-sm text-blue-600 font-medium">
                  <Calendar size={16} />
                  {item.date}
                </div>
                <div className="flex gap-2">
                  {item.tags.map((tag) => (
                    <span
                      key={tag}
                      className="px-2 py-1 text-xs rounded-full bg-gray-100 dark:bg-gray-800 text-gray-600 dark:text-gray-300 border border-gray-200 dark:border-gray-700"
                    >
                      {tag}
                    </span>
                  ))}
                </div>
              </div>

              <h2 className="text-2xl font-bold mb-2">{item.title}</h2>

              <div className="space-y-4">
                <div className="bg-gray-50 dark:bg-gray-950 p-4 rounded-lg border border-gray-200 dark:border-gray-800">
                  <div className="flex items-start gap-3">
                    <Terminal className="mt-1 text-purple-500 shrink-0" size={18} />
                    <div>
                      <span className="text-xs font-semibold text-purple-500 uppercase tracking-wider block mb-1">
                        Prompt
                      </span>
                      <p className="font-mono text-sm text-gray-700 dark:text-gray-300">
                        {item.prompt}
                      </p>
                    </div>
                  </div>
                </div>

                <div className="grid md:grid-cols-2 gap-4">
                  <div>
                    <h3 className="text-sm font-semibold text-red-500 uppercase tracking-wider mb-2">
                      The Problem
                    </h3>
                    <p className="text-gray-600 dark:text-gray-400 text-sm leading-relaxed">
                      {item.problem}
                    </p>
                  </div>
                  <div>
                    <h3 className="text-sm font-semibold text-green-500 uppercase tracking-wider mb-2">
                      The Solution
                    </h3>
                    <div className="text-gray-600 dark:text-gray-400 text-sm leading-relaxed whitespace-pre-line flex gap-2">
                      <CheckCircle2 className="shrink-0 text-green-500" size={16} />
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
