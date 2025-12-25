"use client";

import { Canvas, useFrame } from "@react-three/fiber";
import { useRef, useMemo } from "react";
import * as THREE from "three";
import { OrbitControls } from "@react-three/drei";

interface OrderBookVisualizerProps {
  bids: { price: number; quantity: number }[];
  asks: { price: number; quantity: number }[];
}

function Bar({ position, color, height }: { position: [number, number, number]; color: string; height: number }) {
  const meshRef = useRef<THREE.Mesh>(null);

  // Animate scale on mount/update
  useFrame((state, delta) => {
    if (meshRef.current) {
      meshRef.current.rotation.y += delta * 0.1;
    }
  });

  return (
    <mesh ref={meshRef} position={position}>
      <boxGeometry args={[0.8, height, 0.8]} />
      <meshStandardMaterial color={color} opacity={0.8} transparent />
    </mesh>
  );
}

export default function OrderBookVisualizer({ bids, asks }: OrderBookVisualizerProps) {
  // Normalize heights for visualization
  const maxQty = useMemo(() => {
    const all = [...bids, ...asks];
    return Math.max(...all.map(o => o.quantity), 10);
  }, [bids, asks]);

  return (
    <div className="h-[400px] w-full bg-slate-900 rounded-xl overflow-hidden border border-slate-800">
      <Canvas camera={{ position: [0, 10, 20], fov: 50 }}>
        <ambientLight intensity={0.5} />
        <pointLight position={[10, 10, 10]} intensity={1} />
        <OrbitControls autoRotate autoRotateSpeed={0.5} />
        
        <group position={[-5, 0, 0]}>
          {bids.map((bid, i) => (
            <Bar
              key={`bid-${i}`}
              position={[i * 1.2, (bid.quantity / maxQty) * 5, 2]}
              color="#22c55e" // Green
              height={(bid.quantity / maxQty) * 10}
            />
          ))}
        </group>

        <group position={[-5, 0, 0]}>
          {asks.map((ask, i) => (
            <Bar
              key={`ask-${i}`}
              position={[i * 1.2, (ask.quantity / maxQty) * 5, -2]}
              color="#ef4444" // Red
              height={(ask.quantity / maxQty) * 10}
            />
          ))}
        </group>

        <gridHelper args={[20, 20, 0x444444, 0x222222]} />
      </Canvas>
      <div className="absolute bottom-4 left-4 text-xs text-slate-400">
        3D Order Book Visualization
      </div>
    </div>
  );
}
