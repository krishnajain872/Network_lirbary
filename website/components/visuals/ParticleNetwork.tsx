"use client";

import { useRef, useMemo } from 'react';
import { Canvas, useFrame } from '@react-three/fiber';
import { Points, PointMaterial } from '@react-three/drei';
import * as THREE from 'three';
import { useTheme } from 'next-themes';

function ParticleField({ count = 100, color = "#4f46e5" }) {
  const points = useRef<THREE.Points>(null!);

  // Generate random positions
  const positions = useMemo(() => {
    const pos = new Float32Array(count * 3);
    for (let i = 0; i < count; i++) {
      pos[i * 3] = (Math.random() - 0.5) * 10;     // x
      pos[i * 3 + 1] = (Math.random() - 0.5) * 10; // y
      pos[i * 3 + 2] = (Math.random() - 0.5) * 10; // z
    }
    return pos;
  }, [count]);

  useFrame((state, delta) => {
    if (points.current) {
      points.current.rotation.x -= delta / 10;
      points.current.rotation.y -= delta / 15;
    }
  });

  return (
    <group rotation={[0, 0, Math.PI / 4]}>
      <Points ref={points} positions={positions} stride={3} frustumCulled={false}>
        <PointMaterial
          transparent
          color={color}
          size={0.05}
          sizeAttenuation={true}
          depthWrite={false}
          opacity={0.6}
        />
      </Points>
    </group>
  );
}

// Map themes to particle colors
const themeColors: Record<string, string> = {
   dark: "#3b82f6",      // Blue
   light: "#2563eb",     // Darker Blue
   "high-contrast": "#00ff00", // Green
   oceanic: "#7dd3fc",   // Cyan
   sunset: "#f472b6",    // Pink
   cyberpunk: "#00ff9f", // Neon Green
};

export function ParticleNetwork() {
  const { theme } = useTheme();
  const color = themeColors[theme || 'dark'] || themeColors.dark;

  return (
    <div className="absolute inset-0 -z-10 opacity-30 pointer-events-none">
      <Canvas camera={{ position: [0, 0, 5], fov: 60 }}>
        <ParticleField count={200} color={color} />
      </Canvas>
    </div>
  );
}
