import { Handle, Position } from '@xyflow/react';
import { clsx } from 'clsx';
import { twMerge } from 'tailwind-merge';

interface CurrencyNodeProps {
  data: {
    label: string;
    isCycleNode: boolean;
  };
  selected: boolean;
}

export function CurrencyNode({ data, selected }: CurrencyNodeProps) {
  return (
    <div
      className={twMerge(
        clsx(
          "relative flex items-center justify-center w-[80px] h-[80px] rounded-full border-2 bg-card/90 backdrop-blur-xl transition-all duration-300 flex-shrink-0",
          data.isCycleNode ? "border-primary shadow-[0_0_20px_rgba(0,255,157,0.3)]" : "border-border shadow-md",
          selected && "ring-2 ring-primary/40 scale-105"
        )
      )}
    >
      {/* Invisible handles scattered around so edges route naturally towards the center */}
      <Handle type="target" position={Position.Top} className="opacity-0" />
      <Handle type="source" position={Position.Bottom} className="opacity-0" />
      <Handle type="target" position={Position.Left} className="opacity-0" />
      <Handle type="source" position={Position.Right} className="opacity-0" />

      <div
        className={twMerge(
          clsx(
            "font-mono text-lg font-bold tracking-wider",
            data.isCycleNode ? "text-primary drop-shadow-[0_0_5px_rgba(0,255,157,0.8)]" : "text-muted-foreground"
          )
        )}
      >
        {data.label}
      </div>
      
      {/* Glowing inner dot if part of cycle */}
      {data.isCycleNode && (
        <div className="absolute top-1 right-1 w-2 h-2 rounded-full bg-primary shadow-[0_0_8px_rgba(0,255,157,1)] animate-pulse" />
      )}
    </div>
  );
}
