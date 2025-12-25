"use client";

import { useEffect, useState } from "react";
import OrderBookVisualizer from "./components/OrderBookVisualizer";
import { motion } from "framer-motion";

// Mock data
const INITIAL_BIDS = [
  { price: 100.5, quantity: 500 },
  { price: 100.4, quantity: 300 },
  { price: 100.3, quantity: 700 },
  { price: 100.2, quantity: 200 },
  { price: 100.1, quantity: 450 },
];

const INITIAL_ASKS = [
  { price: 100.6, quantity: 400 },
  { price: 100.7, quantity: 600 },
  { price: 100.8, quantity: 150 },
  { price: 100.9, quantity: 300 },
  { price: 101.0, quantity: 800 },
];

export default function Home() {
  const [bids, setBids] = useState(INITIAL_BIDS);
  const [asks, setAsks] = useState(INITIAL_ASKS);

  // Simulate live updates
  useEffect(() => {
    const interval = setInterval(() => {
      setBids(prev => prev.map(b => ({ ...b, quantity: Math.max(10, b.quantity + (Math.random() - 0.5) * 50) })));
      setAsks(prev => prev.map(a => ({ ...a, quantity: Math.max(10, a.quantity + (Math.random() - 0.5) * 50) })));
    }, 1000);
    return () => clearInterval(interval);
  }, []);

  return (
    <main className="min-h-screen bg-black text-white p-8">
      <div className="max-w-7xl mx-auto space-y-8">
        <header className="flex justify-between items-center border-b border-slate-800 pb-6">
          <div>
            <h1 className="text-4xl font-bold bg-gradient-to-r from-blue-400 to-purple-600 bg-clip-text text-transparent">
              OMS Terminal Pro
            </h1>
            <p className="text-slate-400 mt-2">High-Performance Order Management System</p>
          </div>
          <div className="flex gap-4">
            <div className="px-4 py-2 bg-slate-900 rounded-lg border border-slate-800">
              <span className="text-xs text-slate-500 block">Latency</span>
              <span className="text-green-400 font-mono">0.45 ms</span>
            </div>
            <div className="px-4 py-2 bg-slate-900 rounded-lg border border-slate-800">
              <span className="text-xs text-slate-500 block">Throughput</span>
              <span className="text-blue-400 font-mono">1.2M req/s</span>
            </div>
          </div>
        </header>

        <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
          {/* Order Book 3D Viz */}
          <motion.div 
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            className="lg:col-span-2 space-y-4"
          >
            <h2 className="text-xl font-semibold text-slate-200">Market Depth</h2>
            <OrderBookVisualizer bids={bids} asks={asks} />
          </motion.div>

          {/* Quick Actions / Stats */}
          <motion.div 
             initial={{ opacity: 0, x: 20 }}
             animate={{ opacity: 1, x: 0 }}
             transition={{ delay: 0.2 }}
             className="space-y-6"
          >
            <div className="bg-slate-900/50 backdrop-blur border border-slate-800 rounded-xl p-6">
              <h3 className="text-lg font-medium mb-4">Market Status</h3>
              <div className="space-y-4">
                <div className="flex justify-between items-center">
                  <span className="text-slate-400">Symbol</span>
                  <span className="font-mono font-bold">BTC-USD</span>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-slate-400">24h Vol</span>
                  <span className="font-mono text-slate-200">45,231.54</span>
                </div>
                <div className="h-px bg-slate-800 my-4" />
                <button className="w-full py-3 bg-green-600 hover:bg-green-500 rounded-lg font-semibold transition-colors">
                  Place Bulk Buy
                </button>
                <button className="w-full py-3 bg-red-600 hover:bg-red-500 rounded-lg font-semibold transition-colors">
                  Place Bulk Sell
                </button>
              </div>
            </div>
          </motion.div>
        </div>

        {/* Recent Trades Table */}
        <motion.div
           initial={{ opacity: 0, y: 20 }}
           animate={{ opacity: 1, y: 0 }}
           transition={{ delay: 0.4 }}
           className="bg-slate-900/30 border border-slate-800 rounded-xl overflow-hidden"
        >
          <div className="p-6 border-b border-slate-800">
            <h3 className="text-lg font-medium">Recent Activity</h3>
          </div>
          <table className="w-full text-left text-sm text-slate-400">
            <thead className="bg-slate-900 text-slate-200 uppercase text-xs">
              <tr>
                <th className="px-6 py-4">Time</th>
                <th className="px-6 py-4">Type</th>
                <th className="px-6 py-4">Price</th>
                <th className="px-6 py-4">Amount</th>
                <th className="px-6 py-4">Status</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-800">
              {[...Array(5)].map((_, i) => (
                <tr key={i} className="hover:bg-slate-800/50 transition-colors">
                  <td className="px-6 py-4 font-mono">{new Date().toLocaleTimeString()}</td>
                  <td className={`px-6 py-4 ${i % 2 === 0 ? 'text-green-400' : 'text-red-400'}`}>
                    {i % 2 === 0 ? 'BUY' : 'SELL'}
                  </td>
                  <td className="px-6 py-4 font-mono">100.{50 + i}</td>
                  <td className="px-6 py-4 font-mono">{(Math.random() * 2).toFixed(4)}</td>
                  <td className="px-6 py-4"><span className="px-2 py-1 bg-blue-900/30 text-blue-400 rounded text-xs">FILLED</span></td>
                </tr>
              ))}
            </tbody>
          </table>
        </motion.div>
      </div>
    </main>
  );
}
