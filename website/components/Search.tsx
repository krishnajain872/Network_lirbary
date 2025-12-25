"use client";

import { CommandDialog, CommandEmpty, CommandGroup, CommandInput, CommandItem, CommandList } from "cmdk";
import { useEffect, useState } from "react";
import { useRouter } from "next/navigation";
import {
   BookOpen,
   Cpu,
   Settings,
   Network,
   Code2,
   Home
} from "lucide-react";
import { DialogTitle } from "@radix-ui/react-dialog";

export function Search() {
  const [open, setOpen] = useState(false);
  const router = useRouter();

  useEffect(() => {
    const down = (e: KeyboardEvent) => {
      if (e.key === "k" && (e.metaKey || e.ctrlKey)) {
        e.preventDefault();
        setOpen((open) => !open);
      }
    };
    document.addEventListener("keydown", down);
    return () => document.removeEventListener("keydown", down);
  }, []);

  const runCommand = (command: () => void) => {
    setOpen(false);
    command();
  };

  return (
    <CommandDialog open={open} onOpenChange={setOpen}>
      <DialogTitle className="sr-only">Search Documentation</DialogTitle>
      <div className="fixed inset-0 z-50 flex items-start justify-center pt-[20vh]">
         <div className="w-full max-w-lg overflow-hidden rounded-xl border bg-popover text-popover-foreground shadow-2xl animate-in fade-in-0 zoom-in-95 data-[state=closed]:animate-out data-[state=closed]:fade-out-0 data-[state=closed]:zoom-out-95 duration-200">
            <CommandInput
               placeholder="Type a command or search..."
               className="flex h-12 w-full border-none bg-transparent px-3 py-3 text-sm outline-none placeholder:text-muted-foreground disabled:cursor-not-allowed disabled:opacity-50"
            />
            <CommandList className="max-h-[300px] overflow-y-auto p-2">
               <CommandEmpty className="py-6 text-center text-sm">No results found.</CommandEmpty>

               <CommandGroup heading="Pages">
                  <CommandItem onSelect={() => runCommand(() => router.push('/'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Home className="h-4 w-4" />
                     Home
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/getting-started'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <BookOpen className="h-4 w-4" />
                     Getting Started
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/architecture'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Cpu className="h-4 w-4" />
                     Architecture
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/configuration'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Settings className="h-4 w-4" />
                     Configuration
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/protocols'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Network className="h-4 w-4" />
                     Protocols
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/developer-guide'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Code2 className="h-4 w-4" />
                     Developer Guide
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/use-cases'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <BookOpen className="h-4 w-4" />
                     Use Cases
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/improvements'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Settings className="h-4 w-4" />
                     Improvements
                  </CommandItem>
                  <CommandItem onSelect={() => runCommand(() => router.push('/pros-cons'))} className="flex items-center gap-2 px-2 py-1.5 text-sm rounded-sm hover:bg-accent cursor-pointer">
                     <Cpu className="h-4 w-4" />
                     Pros & Cons
                  </CommandItem>
               </CommandGroup>
            </CommandList>
         </div>
      </div>
    </CommandDialog>
  );
}
