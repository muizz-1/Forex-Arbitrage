import { useEffect, useRef } from 'react';
import cytoscape, { Core, ElementDefinition } from 'cytoscape';
import type { ArbitrageResponse, GraphEdge } from '@workspace/api-client-react';

interface CytoscapeGraphProps {
  currencies: string[];
  data: ArbitrageResponse | undefined;
}

const NODE_SIZE = 80;
const NEON_GREEN = '#00ff9d';
const DIM_EDGE = 'rgba(100,200,150,0.7)';

function buildCycleEdges(data: ArbitrageResponse | undefined): {
  cycleEdgeSet: Set<string>;
  rateMap: Map<string, number>;
} {
  const cycleEdgeSet = new Set<string>();
  const rateMap = new Map<string, number>();

  if (!data) return { cycleEdgeSet, rateMap };

  // Build rate lookup from full edge list
  if (data.edges) {
    for (const e of data.edges) {
      rateMap.set(`${e.from}->${e.to}`, e.rate);
    }
  }

  // Build cycle edge set from consecutive cycle pairs
  const cycle = data.cycle ?? [];
  for (let i = 0; i < cycle.length - 1; i++) {
    cycleEdgeSet.add(`${cycle[i]}->${cycle[i + 1]}`);
  }

  return { cycleEdgeSet, rateMap };
}

function buildElements(
  currencies: string[],
  data: ArbitrageResponse | undefined,
  cy: Core
): ElementDefinition[] {
  const { cycleEdgeSet, rateMap } = buildCycleEdges(data);
  const cycleNodes = new Set(data?.cycle ?? []);

  // Preserve existing node positions for dragged nodes
  const posMap = new Map<string, { x: number; y: number }>();
  cy.nodes().forEach((node) => {
    posMap.set(node.id(), node.position());
  });

  // Circular layout for nodes
  const n = currencies.length;
  const radius = Math.max(180, n * 40);
  const cx = 0;
  const cy_ = 0;

  const nodes: ElementDefinition[] = currencies.map((currency, i) => {
    const angle = (i / n) * 2 * Math.PI - Math.PI / 2;
    const existing = posMap.get(currency);
    return {
      group: 'nodes' as const,
      data: {
        id: currency,
        label: currency,
        isCycle: cycleNodes.has(currency),
      },
      position: existing ?? {
        x: cx + radius * Math.cos(angle),
        y: cy_ + radius * Math.sin(angle),
      },
    };
  });

  // Only show cycle edges
  const edges: ElementDefinition[] = [];
  for (const edgeId of cycleEdgeSet) {
    const [from, to] = edgeId.split('->');
    if (!currencies.includes(from) || !currencies.includes(to)) continue;
    const rate = rateMap.get(edgeId);
    edges.push({
      group: 'edges' as const,
      data: {
        id: edgeId,
        source: from,
        target: to,
        label: rate != null ? rate.toFixed(5) : '',
      },
    });
  }

  return [...nodes, ...edges];
}

export function CytoscapeGraph({ currencies, data }: CytoscapeGraphProps) {
  const containerRef = useRef<HTMLDivElement>(null);
  const cyRef = useRef<Core | null>(null);
  const prevEdgeIds = useRef<Set<string>>(new Set());

  // Init cytoscape once
  useEffect(() => {
    if (!containerRef.current) return;

    const cy = cytoscape({
      container: containerRef.current,
      elements: [],
      style: [
        {
          selector: 'node',
          style: {
            width: NODE_SIZE,
            height: NODE_SIZE,
            shape: 'ellipse',
            'background-color': '#0a0f14',
            'border-width': 2,
            'border-color': '#1e2d2a',
            label: 'data(label)',
            color: '#c0d8cc',
            'font-family': 'JetBrains Mono, monospace',
            'font-size': 14,
            'font-weight': 700,
            'text-valign': 'center',
            'text-halign': 'center',
            'text-outline-width': 0,
            'overlay-opacity': 0,
            'transition-property': 'border-color, background-color, color',
            'transition-duration': 400,
          } as any,
        },
        {
          selector: 'node[?isCycle]',
          style: {
            'background-color': '#0d1f18',
            'border-color': NEON_GREEN,
            'border-width': 2.5,
            color: NEON_GREEN,
          } as any,
        },
        {
          selector: 'edge',
          style: {
            width: 2,
            'line-color': DIM_EDGE,
            'target-arrow-color': DIM_EDGE,
            'target-arrow-shape': 'triangle',
            'arrow-scale': 1.2,
            'curve-style': 'bezier',
            label: 'data(label)',
            color: NEON_GREEN,
            'font-family': 'JetBrains Mono, monospace',
            'font-size': 10,
            'font-weight': 500,
            'text-background-color': '#060d0a',
            'text-background-opacity': 0.9,
            'text-background-padding': '3px',
            'text-background-shape': 'roundrectangle',
            'text-border-color': '#1e3528',
            'text-border-width': 1,
            'text-border-opacity': 1,
            'edge-text-rotation': 'autorotate',
            opacity: 0,
          } as any,
        },
        {
          selector: 'node:selected',
          style: {
            'border-color': '#00ffcc',
            'border-width': 3,
            'overlay-color': '#00ff9d',
            'overlay-opacity': 0.05,
            'overlay-padding': 6,
          } as any,
        },
      ],
      layout: { name: 'preset' },
      userZoomingEnabled: true,
      userPanningEnabled: true,
      boxSelectionEnabled: false,
      autoungrabify: false,
    });

    cyRef.current = cy;

    return () => {
      cy.destroy();
      cyRef.current = null;
    };
  }, []);

  // Update elements when currencies or data changes
  useEffect(() => {
    const cy = cyRef.current;
    if (!cy) return;

    const newElements = buildElements(currencies, data, cy);
    const newEdgeIds = new Set(
      newElements.filter((e) => e.group === 'edges').map((e) => e.data.id as string)
    );

    // Fade out edges that are no longer present
    const removedEdgeIds = [...prevEdgeIds.current].filter((id) => !newEdgeIds.has(id));
    removedEdgeIds.forEach((id) => {
      const edge = cy.getElementById(id);
      if (edge.length) {
        edge.animate({ style: { opacity: 0 } } as any, {
          duration: 350,
          easing: 'ease-in-quad',
          complete: () => cy.remove(edge),
        });
      }
    });

    // Remove nodes no longer in currencies
    cy.nodes().forEach((node) => {
      if (!currencies.includes(node.id())) {
        node.animate({ style: { opacity: 0 } } as any, {
          duration: 300,
          complete: () => cy.remove(node),
        });
      }
    });

    // Add or update nodes
    newElements
      .filter((e) => e.group === 'nodes')
      .forEach((el) => {
        const existing = cy.getElementById(el.data.id as string);
        if (existing.length === 0) {
          const added = cy.add(el);
          added.style('opacity', 0);
          added.animate({ style: { opacity: 1 } } as any, { duration: 400, easing: 'ease-out-quad' });
        } else {
          existing.data('isCycle', el.data.isCycle);
        }
      });

    // Add new cycle edges with fade-in
    newElements
      .filter((e) => e.group === 'edges')
      .forEach((el) => {
        const existing = cy.getElementById(el.data.id as string);
        if (existing.length === 0) {
          const added = cy.add(el);
          added.style('opacity', 0);
          added.animate({ style: { opacity: 1 } } as any, { duration: 500, easing: 'ease-out-quad' });
        } else {
          existing.data('label', el.data.label);
        }
      });

    prevEdgeIds.current = newEdgeIds;

    // Center on first load
    if (cy.nodes().length > 0) {
      cy.fit(cy.nodes(), 80);
    }
  }, [currencies, data]);

  return (
    <div
      ref={containerRef}
      className="w-full h-full"
      style={{ background: 'transparent' }}
    />
  );
}
