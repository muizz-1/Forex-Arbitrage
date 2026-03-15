import { useState, useEffect, useRef, useCallback } from 'react';
import { detectArbitrage } from "@/api/arbitrage"
import type { ArbitrageResponse } from "@/types/arbitrage"

import { CytoscapeGraph } from '@/components/CytoscapeGraph';
import { ControlPanel } from '@/components/ControlPanel';
import { CurrencySidebar } from '@/components/CurrencySidebar';
import { ResultsPanel } from '@/components/ResultsPanel';
import { useToast } from '@/hooks/use-toast';

const POLL_INTERVAL_MS = 10000;
const DEBOUNCE_MS = 500;

export default function Dashboard() {
  const { toast } = useToast();

  const [currencies, setCurrencies] = useState<string[]>([]);
  const [baseCurrency, setBaseCurrency] = useState('');
  const [startingAmount, setStartingAmount] = useState<number>();
  const [txCost, setTxCost] = useState<number>();
  const [bestMode, setBestMode] = useState<boolean>(true);
  const [isLoading, setIsLoading] = useState(false);
  const [data, setData] = useState<ArbitrageResponse | undefined>(undefined);

  // Stable refs for polling
  const currenciesRef = useRef(currencies);
  const baseCurrencyRef = useRef(baseCurrency);
  const startingAmountRef = useRef(startingAmount);
  const txCostRef = useRef(txCost);
  const bestModeRef = useRef(bestMode);
  const intervalRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const debounceRef = useRef<ReturnType<typeof setTimeout> | null>(null);
  const isActiveRef = useRef(false);

  // Keep refs in sync with state
  useEffect(() => { currenciesRef.current = currencies; }, [currencies]);
  useEffect(() => { baseCurrencyRef.current = baseCurrency; }, [baseCurrency]);
  useEffect(() => { startingAmountRef.current = startingAmount; }, [startingAmount]);
  useEffect(() => { txCostRef.current = txCost; }, [txCost]);
  useEffect(() => { bestModeRef.current = bestMode; }, [bestMode]);

  const runOnce = useCallback(async (currencyList?: string[]) => {
    const cur = currencyList ?? currenciesRef.current;
    if (cur.length < 2) return;

    setIsLoading(true);
    try {
      const result = await detectArbitrage({
        currencies: cur,
        base_currency: baseCurrencyRef.current,
        starting_amount: startingAmountRef.current,
        tx_cost: txCostRef.current,
        best_mode: bestModeRef.current,
      });
      setData(result);
    } catch (err: any) {
      toast({
        title: 'Engine Error',
        description: err?.error ?? err?.message ?? 'Failed to reach backend.',
        variant: 'destructive',
      });
    } finally {
      setIsLoading(false);
    }
  }, [toast]);

  const stopPolling = useCallback(() => {
    isActiveRef.current = false;
    if (intervalRef.current) {
      clearInterval(intervalRef.current);
      intervalRef.current = null;
    }
  }, []);

  const startPolling = useCallback((currencyList?: string[]) => {
    stopPolling();
    const cur = currencyList ?? currenciesRef.current;
    if (cur.length < 2) return;

    isActiveRef.current = true;

    // Run immediately, then on interval
    runOnce(cur);
    intervalRef.current = setInterval(() => {
      if (isActiveRef.current) runOnce(currenciesRef.current);
    }, POLL_INTERVAL_MS);
  }, [stopPolling, runOnce]);

  // Start polling on mount
  useEffect(() => {
    startPolling();
    return () => stopPolling();
  // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  // On currency list change: stop, debounce 500ms, restart
  const handleCurrencyChange = useCallback((newCurrencies: string[]) => {
    stopPolling();

    if (debounceRef.current) clearTimeout(debounceRef.current);
    debounceRef.current = setTimeout(() => {
      startPolling(newCurrencies);
    }, DEBOUNCE_MS);
  }, [stopPolling, startPolling]);

  const handleAddCurrency = useCallback((c: string) => {
    setCurrencies((prev) => {
      if (prev.includes(c)) return prev;
      const next = [...prev, c];
      handleCurrencyChange(next);
      return next;
    });
  }, [handleCurrencyChange]);

  const handleRemoveCurrency = useCallback((c: string) => {
    setCurrencies((prev) => {
      const next = prev.filter((x) => x !== c);
      handleCurrencyChange(next);
      return next;
    });
  }, [handleCurrencyChange]);

  // Manual detect — run immediately and reset interval
  const handleManualDetect = useCallback(() => {
    startPolling();
  }, [startPolling]);

  return (
    <div className="flex flex-col w-screen h-screen bg-background overflow-hidden">
      <ControlPanel
        isLoading={isLoading}
        onSubmit={handleManualDetect}
        baseCurrency={baseCurrency}
        onBaseCurrencyChange={setBaseCurrency}
        startingAmount={startingAmount}
        onAmountChange={setStartingAmount}
        txCost={txCost}
        onTxCostChange={setTxCost}
        bestMode={bestMode}
        onBestModeChange={setBestMode}
      />
      <div className="flex flex-row flex-1 overflow-hidden">
        <CurrencySidebar
          currencies={currencies}
          onAdd={handleAddCurrency}
          onRemove={handleRemoveCurrency}
          cycleNodes={data?.cycle ?? []}
        />
        <div className="flex-1 relative overflow-hidden" style={{ background: '#060d0a' }}>
          {/* Subtle grid background */}
          <div
            className="absolute inset-0 pointer-events-none"
            style={{
              backgroundImage:
                'radial-gradient(circle, rgba(255,255,255,0.06) 1px, transparent 1px)',
              backgroundSize: '28px 28px',
            }}
          />
          <CytoscapeGraph currencies={currencies} data={data} />
          <ResultsPanel data={data} isLoading={isLoading} />

          {/* Polling indicator */}
          <div className="absolute bottom-4 left-4 flex items-center gap-2 opacity-40">
            <div
              className={`w-1.5 h-1.5 rounded-full ${isLoading ? 'bg-yellow-400 animate-pulse' : 'bg-green-400 animate-pulse'}`}
            />
            <span className="text-xs font-mono text-white">
              {isLoading ? 'scanning...' : `polling every ${POLL_INTERVAL_MS / 1000}s`}
            </span>
          </div>
        </div>
      </div>
    </div>
  );
}
