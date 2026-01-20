import { useState } from 'react';

interface ProgrammerCalculatorProps {
  display: string;
  onButtonClick: (value: string) => void;
  onClear: () => void;
  onDelete: () => void;
  onEquals: () => void;
}

export function ProgrammerCalculator({
  display,
  onButtonClick,
  onClear,
  onDelete,
  onEquals,
}: ProgrammerCalculatorProps) {
  const [mode, setMode] = useState<'HEX' | 'DEC' | 'OCT' | 'BIN'>('DEC');

  const buttons = [
    ['A', 'B', 'C', 'D', 'E', 'F'],
    ['CE', 'C', '⌫', '÷', '×', '-'],
    ['7', '8', '9', 'OR', 'XOR', 'AND'],
    ['4', '5', '6', '<<', '>>', 'NOT'],
    ['1', '2', '3', 'MOD', '(', ')'],
    ['0', '.', '=', '+', 'NEG', 'CLR'],
  ];

  const handleClick = (value: string) => {
    if (value === 'C' || value === 'CLR') {
      onClear();
    } else if (value === 'CE') {
      onClear();
    } else if (value === '⌫') {
      onDelete();
    } else if (value === '=') {
      onEquals();
    } else {
      onButtonClick(value);
    }
  };

  const getButtonStyle = (value: string) => {
    const hexOnly = ['A', 'B', 'C', 'D', 'E', 'F'];
    const octalInvalid = ['8', '9'];
    const binInvalid = ['2', '3', '4', '5', '6', '7', '8', '9'];
    
    let disabled = false;
    if (mode === 'DEC' && hexOnly.includes(value)) disabled = true;
    if (mode === 'OCT' && (hexOnly.includes(value) || octalInvalid.includes(value))) disabled = true;
    if (mode === 'BIN' && (hexOnly.includes(value) || binInvalid.includes(value))) disabled = true;
    
    if (disabled) {
      return 'bg-gray-200 text-gray-400 dark:bg-gray-800 dark:text-gray-600 cursor-not-allowed opacity-50';
    }
    
    if (value === '=') {
      return 'bg-gradient-to-br from-blue-600 to-indigo-600 hover:from-blue-700 hover:to-indigo-700 text-white shadow-lg shadow-blue-500/30 hover:shadow-xl hover:shadow-blue-500/40 hover:scale-105';
    }
    if (['+', '-', '×', '÷'].includes(value)) {
      return 'bg-gradient-to-br from-orange-500 to-pink-500 hover:from-orange-600 hover:to-pink-600 text-white shadow-md shadow-orange-500/20 hover:shadow-lg hover:shadow-orange-500/30 hover:scale-105';
    }
    if (['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'A', 'B', 'C', 'D', 'E', 'F'].includes(value)) {
      return 'bg-white dark:bg-gray-800 hover:bg-gray-50 dark:hover:bg-gray-700 shadow-md hover:shadow-lg hover:scale-105 border border-gray-200/50 dark:border-gray-700/50';
    }
    return 'bg-gradient-to-br from-teal-100 to-cyan-100 hover:from-teal-200 hover:to-cyan-200 dark:from-gray-700 dark:to-gray-800 dark:hover:from-gray-600 dark:hover:to-gray-700 text-sm shadow-md hover:shadow-lg hover:scale-105';
  };

  return (
    <div className="flex flex-col gap-3">
      <div className="flex gap-2">
        {(['HEX', 'DEC', 'OCT', 'BIN'] as const).map((m) => (
          <button
            key={m}
            onClick={() => setMode(m)}
            className={`flex-1 py-3 rounded-2xl font-bold transition-all duration-300 ${
              mode === m
                ? 'bg-gradient-to-r from-blue-600 to-indigo-600 text-white shadow-lg shadow-blue-500/30 scale-105'
                : 'bg-gray-200 hover:bg-gray-300 dark:bg-gray-700 dark:hover:bg-gray-600 shadow-md hover:shadow-lg hover:scale-105'
            }`}
          >
            {m}
          </button>
        ))}
      </div>
      
      <div className="flex flex-col gap-2">
        {buttons.map((row, rowIndex) => (
          <div key={rowIndex} className="grid grid-cols-6 gap-2">
            {row.map((btn) => (
              <button
                key={btn}
                onClick={() => handleClick(btn)}
                className={`h-14 rounded-2xl font-semibold transition-all duration-200 ${getButtonStyle(
                  btn
                )}`}
                disabled={getButtonStyle(btn).includes('cursor-not-allowed')}
              >
                {btn}
              </button>
            ))}
          </div>
        ))}
      </div>
    </div>
  );
}