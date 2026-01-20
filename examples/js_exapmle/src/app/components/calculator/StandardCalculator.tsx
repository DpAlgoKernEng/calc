import { Calculator } from 'lucide-react';

interface StandardCalculatorProps {
  display: string;
  onButtonClick: (value: string) => void;
  onClear: () => void;
  onDelete: () => void;
  onEquals: () => void;
}

export function StandardCalculator({
  display,
  onButtonClick,
  onClear,
  onDelete,
  onEquals,
}: StandardCalculatorProps) {
  const buttons = [
    ['%', 'CE', 'C', '⌫'],
    ['1/x', 'x²', '√', '÷'],
    ['7', '8', '9', '×'],
    ['4', '5', '6', '-'],
    ['1', '2', '3', '+'],
    ['±', '0', '.', '='],
  ];

  const handleClick = (value: string) => {
    if (value === 'C') {
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
    if (value === '=') {
      return 'bg-gradient-to-br from-blue-600 to-indigo-600 hover:from-blue-700 hover:to-indigo-700 text-white col-span-1 shadow-lg shadow-blue-500/30 hover:shadow-xl hover:shadow-blue-500/40 hover:scale-105';
    }
    if (['+', '-', '×', '÷'].includes(value)) {
      return 'bg-gradient-to-br from-orange-500 to-pink-500 hover:from-orange-600 hover:to-pink-600 text-white shadow-md shadow-orange-500/20 hover:shadow-lg hover:shadow-orange-500/30 hover:scale-105';
    }
    if (['%', 'CE', 'C', '⌫', '1/x', 'x²', '√', '±'].includes(value)) {
      return 'bg-gradient-to-br from-gray-200 to-gray-300 hover:from-gray-300 hover:to-gray-400 dark:from-gray-700 dark:to-gray-800 dark:hover:from-gray-600 dark:hover:to-gray-700 shadow-md hover:shadow-lg hover:scale-105';
    }
    return 'bg-white dark:bg-gray-800 hover:bg-gray-50 dark:hover:bg-gray-700 shadow-md hover:shadow-lg hover:scale-105 border border-gray-200/50 dark:border-gray-700/50';
  };

  return (
    <div className="flex flex-col gap-3">
      {buttons.map((row, rowIndex) => (
        <div key={rowIndex} className="grid grid-cols-4 gap-3">
          {row.map((btn) => (
            <button
              key={btn}
              onClick={() => handleClick(btn)}
              className={`h-16 rounded-2xl text-lg font-semibold transition-all duration-200 ${getButtonStyle(
                btn
              )}`}
            >
              {btn}
            </button>
          ))}
        </div>
      ))}
    </div>
  );
}