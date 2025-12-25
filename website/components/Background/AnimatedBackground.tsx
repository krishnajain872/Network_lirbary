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
      dummy.position.set(
        position[0],
        position[1] + Math.sin(state.clock.elapsedTime * speed + offset) * 0.5,
        position[2]
      );
      dummy.rotation.x += speed;
      dummy.rotation.y += speed;
      dummy.updateMatrix();
      mesh.current!.setMatrixAt(i, dummy.matrix);
    });
    mesh.current.instanceMatrix.needsUpdate = true;
  });

  const color = theme === "dark" ? "#4f46e5" : theme === "sunset" ? "#ea580c" : "#0ea5e9";

  return (
    <instancedMesh ref={mesh} args={[undefined, undefined, count]}>
      <octahedronGeometry args={[0.2, 0]} />
      <meshStandardMaterial color={color} transparent opacity={0.6} />
    </instancedMesh>
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
