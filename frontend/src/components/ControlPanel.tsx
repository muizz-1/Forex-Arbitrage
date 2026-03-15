import { Play, Loader2 } from 'lucide-react';

interface ControlPanelProps {
  isLoading: boolean;
  onSubmit: () => void;
  baseCurrency: string;
  onBaseCurrencyChange: (val: string) => void;
  startingAmount: number;
  onAmountChange: (val: number) => void;
  txCost: number;
  onTxCostChange: (val: number) => void;
  bestMode: boolean;
  onBestModeChange: (val: boolean) => void;
}

export function ControlPanel({
  isLoading,
  onSubmit,
  baseCurrency,
  onBaseCurrencyChange,
  startingAmount,
  onAmountChange,
  txCost,
  onTxCostChange,
  bestMode,
  onBestModeChange
}: ControlPanelProps) {
  return (
    <div className="w-full h-14 bg-card/80 backdrop-blur-md border-b border-border flex items-center justify-between px-6 z-50 shadow-md flex-shrink-0">
      {/* Logo Area */}
      <div className="flex items-center space-x-3">
        <div className="w-2.5 h-2.5 rounded-full bg-primary animate-pulse shadow-[0_0_8px_rgba(0,255,157,0.8)]" />
        <h1 className="font-mono font-bold text-sm tracking-[0.2em] text-foreground">
          FOREX ARB DETECTOR
        </h1>
      </div>

      {/* Controls */}
      <div className="flex items-center space-x-6">
        <div className="flex items-center space-x-2">
          <label className="text-xs font-semibold text-muted-foreground uppercase tracking-wider">Base</label>
          <input
            type="text"
            value={baseCurrency}
            onChange={(e) => onBaseCurrencyChange(e.target.value.toUpperCase())}
            className="w-16 px-2 py-1 bg-background border border-border rounded text-sm font-mono text-center focus:outline-none focus:border-primary/50 uppercase"
          />
        </div>

        <div className="flex items-center space-x-2">
          <label className="text-xs font-semibold text-muted-foreground uppercase tracking-wider">Amount</label>
          <input
            type="number"
            value={startingAmount}
            onChange={(e) => onAmountChange(Number(e.target.value))}
            className="w-24 px-2 py-1 bg-background border border-border rounded text-sm font-mono text-right focus:outline-none focus:border-primary/50"
          />
        </div>

        <div className="flex items-center space-x-2">
          <label className="text-xs font-semibold text-muted-foreground uppercase tracking-wider">Tx Cost</label>
          <input
            type="number"
            step="0.0001"
            value={txCost}
            onChange={(e) => onTxCostChange(Number(e.target.value))}
            className="w-24 px-2 py-1 bg-background border border-border rounded text-sm font-mono text-right focus:outline-none focus:border-primary/50"
          />
        </div>

        <div className="flex items-center space-x-3">
          <label className="text-xs font-semibold text-muted-foreground uppercase tracking-wider cursor-pointer" onClick={() => onBestModeChange(!bestMode)}>Best Cycle</label>
          <button
            type="button"
            onClick={() => onBestModeChange(!bestMode)}
            className={`relative inline-flex h-5 w-9 items-center rounded-full transition-colors duration-300 focus:outline-none ${bestMode ? 'bg-primary shadow-[0_0_8px_rgba(0,255,157,0.4)]' : 'bg-muted'}`}
          >
            <span className={`inline-block h-3.5 w-3.5 transform rounded-full bg-white transition-transform duration-300 ${bestMode ? 'translate-x-4' : 'translate-x-1'}`} />
          </button>
        </div>
      </div>

      {/* Detect Button */}
      <button
        onClick={onSubmit}
        disabled={isLoading}
        className="flex items-center space-x-2 px-5 py-2 bg-primary/10 hover:bg-primary/20 text-primary border border-primary/50 rounded hover:border-primary font-mono font-bold text-sm tracking-wider transition-all duration-300 disabled:opacity-50 disabled:cursor-not-allowed hover:shadow-[0_0_15px_rgba(0,255,157,0.3)]"
      >
        {isLoading ? (
          <>
            <Loader2 className="w-4 h-4 animate-spin" />
            <span>DETECTING...</span>
          </>
        ) : (
          <>
            <Play className="w-4 h-4 fill-current" />
            <span>DETECT</span>
          </>
        )}
      </button>
    </div>
  );
}
