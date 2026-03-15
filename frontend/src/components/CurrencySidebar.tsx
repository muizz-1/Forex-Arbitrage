import { useState } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { Plus, X } from 'lucide-react';

interface CurrencySidebarProps {
  currencies: string[];
  onAdd: (c: string) => void;
  onRemove: (c: string) => void;
  cycleNodes: string[];
}

export function CurrencySidebar({ currencies, onAdd, onRemove, cycleNodes }: CurrencySidebarProps) {
  const [newCurrency, setNewCurrency] = useState('');

  const handleAdd = () => {
    const trimmed = newCurrency.trim().toUpperCase();
    if (trimmed && trimmed.length > 0) {
      onAdd(trimmed);
      setNewCurrency('');
    }
  };

  const handleKeyDown = (e: React.KeyboardEvent) => {
    if (e.key === 'Enter') {
      handleAdd();
    }
  };

  return (
    <div className="w-40 h-full bg-card/50 border-r border-border flex flex-col backdrop-blur-sm z-10">
      <div className="p-3 border-b border-border flex gap-2">
        <input
          type="text"
          value={newCurrency}
          onChange={(e) => setNewCurrency(e.target.value)}
          onKeyDown={handleKeyDown}
          placeholder="EUR"
          className="w-full bg-background/50 border border-border rounded px-2 py-1 text-sm font-mono focus:outline-none focus:border-primary/50 text-foreground uppercase"
        />
        <button
          onClick={handleAdd}
          className="p-1 rounded bg-primary/20 hover:bg-primary/30 text-primary transition-colors flex items-center justify-center border border-primary/20 hover:border-primary/50"
        >
          <Plus className="w-4 h-4" />
        </button>
      </div>
      <div className="flex-1 overflow-y-auto p-3 space-y-2 custom-scrollbar">
        <AnimatePresence>
          {currencies.map((currency) => {
            const isCycle = cycleNodes.includes(currency);
            return (
              <motion.div
                key={currency}
                initial={{ opacity: 0, x: -20 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -20 }}
                transition={{ duration: 0.2 }}
                className={`flex items-center justify-between px-3 py-2 rounded-md border ${
                  isCycle 
                    ? 'bg-primary/10 border-primary text-primary shadow-[0_0_10px_rgba(0,255,157,0.2)]' 
                    : 'bg-background/50 border-border text-foreground hover:border-white/20'
                } transition-all duration-300`}
              >
                <span className="font-mono font-bold text-sm tracking-wider">
                  {currency}
                </span>
                <button
                  onClick={() => onRemove(currency)}
                  className={`p-0.5 rounded-full ${isCycle ? 'hover:bg-primary/20' : 'hover:bg-white/10'} transition-colors`}
                >
                  <X className="w-3.5 h-3.5" />
                </button>
              </motion.div>
            );
          })}
        </AnimatePresence>
      </div>
    </div>
  );
}
