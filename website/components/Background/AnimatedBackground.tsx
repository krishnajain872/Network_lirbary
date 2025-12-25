"use client";

import { Canvas, useFrame } from "@react-three/fiber";
import { useRef } from "react";
import * as THREE from "three";
import { useTheme } from "next-themes";

function Particles({ count = 200 }) {
  const mesh = useRef<THREE.InstancedMesh>(null);
  const { theme } = useTheme();

  // Generate random positions
  const dummy = new THREE.Object3D();
  const particles = new Array(count).fill(0).map(() => ({
    position: [
      (Math.random() - 0.5) * 20,
      (Math.random() - 0.5) * 20,
      (Math.random() - 0.5) * 10 - 5,
    ],
    speed: Math.random() * 0.02,
    offset: Math.random() * 100,
  }));

  useFrame((state) => {
    if (!mesh.current) return;

    particles.forEach((particle, i) => {
      const { position, speed, offset } = particle;
      // Simple floating animation
      const y = position[1] + Math.sin(state.clock.elapsedTime * speed + offset) * 0.5;

      dummy.position.set(position[0], y, position[2]);

      // Rotate based on theme
      dummy.rotation.x += speed;
      dummy.rotation.y += speed;

      // Scale pulsing
      const scale = 1 + Math.sin(state.clock.elapsedTime * 2 + offset) * 0.2;
      dummy.scale.set(scale, scale, scale);

      dummy.updateMatrix();
      mesh.current!.setMatrixAt(i, dummy.matrix);
    });
    mesh.current.instanceMatrix.needsUpdate = true;
  });

  // Theme-aware colors
  const getColor = () => {
      switch(theme) {
          case 'dark': return "#4f46e5"; // Indigo
          case 'oceanic': return "#0ea5e9"; // Sky
          case 'sunset': return "#ea580c"; // Orange
          case 'cyberpunk': return "#ff00ff"; // Magenta
          case 'high-contrast': return "#ffffff"; // White
          default: return "#2563eb"; // Blue
      }
  };

  return (
    <group>
        <instancedMesh ref={mesh} args={[undefined, undefined, count]}>
            <dodecahedronGeometry args={[0.15, 0]} />
            <meshStandardMaterial color={getColor()} transparent opacity={0.4} roughness={0.2} metalness={0.8} />
        </instancedMesh>
    </group>
  );
}

export default function AnimatedBackground() {
  return (
    <div className="fixed inset-0 -z-10 h-screen w-full pointer-events-none opacity-50">
      <Canvas camera={{ position: [0, 0, 10], fov: 75 }}>
        <ambientLight intensity={0.5} />
        <pointLight position={[10, 10, 10]} />
        <Particles />
      </Canvas>
    </div>
  );
}
