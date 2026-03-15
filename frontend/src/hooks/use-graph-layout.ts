import { useMemo } from 'react';
import type { Node, Edge } from '@xyflow/react';
import type { ArbitrageResponse } from '@workspace/api-client-react';

export function useGraphLayout(data: ArbitrageResponse | undefined) {
  return useMemo(() => {
    if (!data || !data.nodes || !data.edges) {
      return { nodes: [], edges: [] };
    }

    // 1. Circular Layout for Nodes
    const radius = Math.max(250, data.nodes.length * 35);
    const flowNodes: Node[] = data.nodes.map((node, index) => {
      const angle = (index / data.nodes.length) * 2 * Math.PI;
      const x = radius * Math.cos(angle);
      const y = radius * Math.sin(angle);

      // Check if node is part of the arbitrage cycle
      const isCycleNode = data.cycle?.includes(node) ?? false;

      return {
        id: node,
        position: { x, y },
        data: { label: node, isCycleNode },
        type: 'currency',
      };
    });

    // 2. Identify Cycle Edges
    const cycleEdges = new Set<string>();
    if (data.cycle && data.cycle.length > 1) {
      for (let i = 0; i < data.cycle.length - 1; i++) {
        const from = data.cycle[i];
        const to = data.cycle[i + 1];
        cycleEdges.add(`${from}->${to}`);
      }
    }

    // 3. Map Edges
    const flowEdges: Edge[] = data.edges.map((edge) => {
      const edgeId = `${edge.from}->${edge.to}`;
      const isCycleEdge = cycleEdges.has(edgeId);

      return {
        id: edgeId,
        source: edge.from,
        target: edge.to,
        label: edge.rate.toFixed(5),
        animated: isCycleEdge,
        zIndex: isCycleEdge ? 100 : 0,
        style: {
          stroke: isCycleEdge ? 'hsl(var(--primary))' : 'hsl(var(--muted-foreground) / 0.3)',
          strokeWidth: isCycleEdge ? 3 : 1.5,
          opacity: isCycleEdge ? 1 : 0.4,
        },
        labelStyle: {
          fill: isCycleEdge ? 'hsl(var(--primary))' : 'hsl(var(--muted-foreground))',
          fontWeight: isCycleEdge ? 700 : 400,
          fontFamily: 'JetBrains Mono',
          fontSize: isCycleEdge ? 12 : 10,
        },
        labelBgStyle: {
          fill: 'hsl(var(--background))',
          fillOpacity: 0.9,
          stroke: isCycleEdge ? 'hsl(var(--primary) / 0.5)' : 'hsl(var(--border))',
          strokeWidth: 1,
          rx: 4,
          ry: 4,
        },
        labelBgPadding: [4, 2],
      };
    });

    return { nodes: flowNodes, edges: flowEdges };
  }, [data]);
}
