interface ScientificCalculatorProps {
  display: string;
  onButtonClick: (value: string) => void;
  onClear: () => void;
  onDelete: () => void;
  onEquals: () => void;
}

export function ScientificCalculator({
  display,
  onButtonClick,
  onClear,
  onDelete,
  onEquals,
}: ScientificCalculatorProps) {
  const buttons = [
    ['(', ')', '%', 'CE', 'C', '⌫'],
    ['inv', 'sin', 'ln', '7', '8', '9', '÷'],
    ['π', 'cos', 'log', '4', '5', '6', '×'],
    ['e', 'tan', '√', '1', '2', '3', '-'],
    ['x²', 'x³', 'xʸ', '0', '.', '=', '+'],
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
      return 'bg-gradient-to-br from-blue-600 to-indigo-600 hover:from-blue-700 hover:to-indigo-700 text-white shadow-lg shadow-blue-500/30 hover:shadow-xl hover:shadow-blue-500/40 hover:scale-105';
    }
    if (['+', '-', '×', '÷'].includes(value)) {
      return 'bg-gradient-to-br from-orange-500 to-pink-500 hover:from-orange-600 hover:to-pink-600 text-white shadow-md shadow-orange-500/20 hover:shadow-lg hover:shadow-orange-500/30 hover:scale-105';
    }
    if (['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'].includes(value)) {
      return 'bg-white dark:bg-gray-800 hover:bg-gray-50 dark:hover:bg-gray-700 shadow-md hover:shadow-lg hover:scale-105 border border-gray-200/50 dark:border-gray-700/50';
    }
    return 'bg-gradient-to-br from-purple-100 to-indigo-100 hover:from-purple-200 hover:to-indigo-200 dark:from-gray-700 dark:to-gray-800 dark:hover:from-gray-600 dark:hover:to-gray-700 text-sm shadow-md hover:shadow-lg hover:scale-105';
  };

  return (
    <div className="flex flex-col gap-3">
      {buttons.map((row, rowIndex) => (
        <div key={rowIndex} className="grid grid-cols-7 gap-2">
          {row.map((btn) => (
            <button
              key={btn}
              onClick={() => handleClick(btn)}
              className={`h-14 rounded-2xl font-semibold transition-all duration-200 ${getButtonStyle(
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