export const julesJourneyData = [
  {
    id: "1",
    date: "2025-02-18",
    title: "Compilation & Docker Fixes",
    prompt: "FIX the Compilation issues and Dockerization",
    problem: "The project had missing dependencies, namespace mismatches (networklib::core::security vs networklib::security), missing includes for Linux compilation (<sys/epoll.h>), and incorrect linking in CMake.",
    solution: "1. Refactored CMakeLists.txt to build a shared library `networklib`.\n2. Linked all executables to this library.\n3. Fixed C++ namespace errors.\n4. Implemented correct QUIC handler interface.\n5. Verified Docker build with multi-stage caching.",
    tags: ["C++", "CMake", "Docker", "Refactoring"]
  },
  {
      id: "2",
      date: "2025-02-18",
      title: "Documentation Website",
      prompt: "Create a full-stack Next.js documentation website with 3D animations, multiple themes, and a section for AI development journey.",
      problem: "Need a modern, attractive documentation platform to showcase the library and the AI development process.",
      solution: "Scaffolded a Next.js 14+ app with Tailwind CSS, Framer Motion, and React Three Fiber. Implemented a responsive layout, theme switcher (Dark, Light, Oceanic, Sunset), and this dynamic journey timeline.",
      tags: ["Next.js", "React", "UI/UX", "3D"]
  }
];
