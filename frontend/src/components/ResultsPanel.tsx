import { useRef } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { TrendingUp, AlertCircle, ArrowRight, GripHorizontal } from 'lucide-react';
import type { ArbitrageResponse } from '@workspace/api-client-react';

interface ResultsPanelProps {
  data?: ArbitrageResponse;
  isLoading: boolean;
}

export function ResultsPanel({ data, isLoading }: ResultsPanelProps) {
  const constraintsRef = useRef(null);

  const hasCycle = data?.cycle && data.cycle.length > 1;

  // Support both response formats:
  // New: { cycle, profit_before, profit_after, start, end }
  // Old: { found, cycle, profit, profit_amount }
  const profitBefore =
    data?.profit_before_cost != null
      ? data.profit_before_cost 
      : null;

  const profitAfter =
    data?.profit_after_cost != null
      ? data.profit_after_cost 
      : null;

  const startAmount = data?.start_amount ?? null;
  const endAmount = data?.end_amount ?? null;

  const profitMultiplier =
    startAmount != null && endAmount != null && startAmount > 0
      ? endAmount / startAmount
      : null;
  return (





    <div ref={constraintsRef} className="absolute inset-0 pointer-events-none">
      <AnimatePresence>
        {(data || isLoading) && (
          <motion.div
            key="results"
            initial={{ opacity: 0, y: -12, scale: 0.97 }}
            animate={{ opacity: 1, y: 0, scale: 1 }}
            exit={{ opacity: 0, y: -12, scale: 0.97 }}
            transition={{ duration: 0.3, ease: 'easeOut' }}
            drag
            dragMomentum={false}
            dragConstraints={constraintsRef}
            className="absolute top-4 right-4 z-50 w-72 pointer-events-auto cursor-default"
            style={{ touchAction: 'none' }}
          >
            {/* Glass panel */}
            <div
              className="rounded-2xl overflow-hidden"
              style={{
                background: 'rgba(6, 15, 11, 0.55)',
                backdropFilter: 'blur(20px)',
                WebkitBackdropFilter: 'blur(20px)',
                border: '1px solid rgba(0, 255, 157, 0.15)',
                boxShadow: '0 8px 32px rgba(0,0,0,0.5), inset 0 1px 0 rgba(255,255,255,0.04)',
              }}
            >
              {/* Drag handle header */}
              <div
                className="flex items-center justify-between px-4 py-2.5 cursor-grab active:cursor-grabbing border-b"
                style={{ borderColor: 'rgba(0,255,157,0.1)' }}
                onPointerDown={(e) => e.currentTarget.closest('[data-framer-component-type]') && e.stopPropagation()}
              >
                <div className="flex items-center gap-2">
                  <TrendingUp className="w-3.5 h-3.5" style={{ color: '#00ff9d' }} />
                  <span className="text-xs font-bold tracking-widest uppercase font-mono" style={{ color: '#00ff9d' }}>
                    Detection Results
                  </span>
                </div>
                <GripHorizontal className="w-4 h-4 opacity-30" style={{ color: '#00ff9d' }} />
              </div>

              <div
                className="p-4"
                onPointerDown={(e) => e.stopPropagation()}
              >
                <AnimatePresence mode="wait">
                  {isLoading ? (
                    <motion.div
                      key="loading"
                      initial={{ opacity: 0 }}
                      animate={{ opacity: 1 }}
                      exit={{ opacity: 0 }}
                      className="flex flex-col gap-2"
                    >
                      {[1, 2, 3].map((i) => (
                        <div
                          key={i}
                          className="h-5 rounded animate-pulse"
                          style={{ background: 'rgba(0,255,157,0.06)', width: `${70 - i * 10}%` }}
                        />
                      ))}
                    </motion.div>
                  ) : hasCycle ? (
                    <motion.div
                      key="found"
                      initial={{ opacity: 0, y: 6 }}
                      animate={{ opacity: 1, y: 0 }}
                      exit={{ opacity: 0, y: 6 }}
                      className="space-y-4"
                    >
                      {/* Profit figures */}
                      <div className="grid grid-cols-2 gap-2">
                        {profitAfter != null && (
                          <div
                            className="rounded-xl p-3 flex flex-col"
                            style={{ background: 'rgba(0,255,157,0.07)', border: '1px solid rgba(0,255,157,0.18)' }}
                          >
                            <span className="text-xs font-mono opacity-60 mb-1" style={{ color: '#00ff9d' }}>
                              PROFIT
                            </span>
                            <span className="text-lg font-mono font-bold" style={{ color: '#00ff9d' }}>
                                +{typeof profitAfter === 'number' ? profitAfter.toFixed(4) + " %" : '—'}
                            </span>
                          </div>
                        )}
                        {profitMultiplier != null && (
                          <div
                            className="rounded-xl p-3 flex flex-col"
                            style={{ background: 'rgba(255,255,255,0.03)', border: '1px solid rgba(255,255,255,0.06)' }}
                          >
                            <span className="text-xs font-mono opacity-50 mb-1 text-white">MULT</span>
                            <span className="text-lg font-mono font-bold text-white">
                              {profitMultiplier.toFixed(5)}x
                            </span>
                          </div>
                        )}
                        {endAmount != null && startAmount != null && (
                          <div
                            className="col-span-2 rounded-xl p-3 flex flex-col"
                            style={{ background: 'rgba(255,255,255,0.02)', border: '1px solid rgba(255,255,255,0.05)' }}
                          >
                            <span className="text-xs font-mono opacity-50 mb-1 text-white">
                              {startAmount.toFixed(0)} → {endAmount.toFixed(2)}
                            </span>
                          </div>
                        )}
                      </div>

                      {/* Cycle path */}
                      <div>
                        <p className="text-xs font-mono uppercase tracking-widest opacity-40 text-white mb-2">
                          Cycle
                        </p>
                        <div className="flex flex-wrap items-center gap-1.5">
                          {data!.cycle!.map((node, i) => (
                            <div key={`${node}-${i}`} className="flex items-center gap-1">
                              <span
                                className="px-2 py-0.5 rounded font-mono text-xs font-bold"
                                style={{
                                  background: 'rgba(0,255,157,0.1)',
                                  border: '1px solid rgba(0,255,157,0.3)',
                                  color: '#00ff9d',
                                }}
                              >
                                {node}
                              </span>
                              {i < data!.cycle!.length - 1 && (
                                <ArrowRight className="w-3 h-3 opacity-40" style={{ color: '#00ff9d' }} />
                              )}
                            </div>
                          ))}
                        </div>
                      </div>
                    </motion.div>
                  ) : (
                    <motion.div
                      key="not-found"
                      initial={{ opacity: 0 }}
                      animate={{ opacity: 1 }}
                      exit={{ opacity: 0 }}
                      className="flex flex-col items-center py-5 text-center gap-3"
                    >
                      <AlertCircle className="w-8 h-8 opacity-40" style={{ color: '#ff4d4d' }} />
                      <div>
                        <p className="text-sm font-mono font-bold text-white opacity-60">No opportunity found</p>
                        <p className="text-xs font-mono opacity-30 text-white mt-1">Try different currencies</p>
                      </div>
                    </motion.div>
                  )}
                </AnimatePresence>
              </div>
            </div>
          </motion.div>
        )}
      </AnimatePresence>
    </div>
  );
}
