import { useState } from 'react';
import { Calculator, FlaskConical, Binary, History } from 'lucide-react';
import { StandardCalculator } from '@/app/components/calculator/StandardCalculator';
import { ScientificCalculator } from '@/app/components/calculator/ScientificCalculator';
import { ProgrammerCalculator } from '@/app/components/calculator/ProgrammerCalculator';
import { HistoryPanel, HistoryItem } from '@/app/components/calculator/HistoryPanel';

type CalculatorMode = 'standard' | 'scientific' | 'programmer';

export default function App() {
  const [mode, setMode] = useState<CalculatorMode>('standard');
  const [display, setDisplay] = useState('0');
  const [expression, setExpression] = useState('');
  const [history, setHistory] = useState<HistoryItem[]>([]);
  const [showHistory, setShowHistory] = useState(false);

  const handleButtonClick = (value: string) => {
    // 处理特殊运算符
    const operatorMap: { [key: string]: string } = {
      '×': '*',
      '÷': '/',
      'x²': '^2',
      'x³': '^3',
      '√': 'sqrt(',
      'π': Math.PI.toString(),
      'e': Math.E.toString(),
      '±': '-',
    };

    let processedValue = operatorMap[value] || value;

    if (display === '0' && !isNaN(Number(processedValue))) {
      setDisplay(processedValue);
      setExpression(processedValue);
    } else {
      if (value === '√') {
        setExpression(expression + processedValue);
        setDisplay(display + value);
      } else if (value === 'x²') {
        setExpression(expression + '^2');
        setDisplay(display + '²');
      } else if (value === 'x³') {
        setExpression(expression + '^3');
        setDisplay(display + '³');
      } else if (value === '1/x') {
        setExpression('1/(' + expression + ')');
        setDisplay('1/(' + display + ')');
      } else if (value === '±') {
        if (display.startsWith('-')) {
          setDisplay(display.substring(1));
          setExpression(expression.substring(1));
        } else {
          setDisplay('-' + display);
          setExpression('-' + expression);
        }
      } else {
        setDisplay(display + value);
        setExpression(expression + processedValue);
      }
    }
  };

  const handleClear = () => {
    setDisplay('0');
    setExpression('');
  };

  const handleDelete = () => {
    if (display.length > 1) {
      setDisplay(display.slice(0, -1));
      setExpression(expression.slice(0, -1));
    } else {
      setDisplay('0');
      setExpression('');
    }
  };

  const handleEquals = () => {
    try {
      let evalExpression = expression;
      
      // 处理科学函数
      evalExpression = evalExpression.replace(/sin\(/g, 'Math.sin(');
      evalExpression = evalExpression.replace(/cos\(/g, 'Math.cos(');
      evalExpression = evalExpression.replace(/tan\(/g, 'Math.tan(');
      evalExpression = evalExpression.replace(/ln\(/g, 'Math.log(');
      evalExpression = evalExpression.replace(/log\(/g, 'Math.log10(');
      evalExpression = evalExpression.replace(/sqrt\(/g, 'Math.sqrt(');
      evalExpression = evalExpression.replace(/\^/g, '**');
      
      // 处理程序员模式的位运算
      evalExpression = evalExpression.replace(/OR/g, '|');
      evalExpression = evalExpression.replace(/AND/g, '&');
      evalExpression = evalExpression.replace(/XOR/g, '^');
      evalExpression = evalExpression.replace(/NOT/g, '~');
      evalExpression = evalExpression.replace(/<</g, '<<');
      evalExpression = evalExpression.replace(/>>/g, '>>');
      evalExpression = evalExpression.replace(/MOD/g, '%');
      
      const result = eval(evalExpression);
      const resultStr = result.toString();
      
      // 添加到历史记录
      const historyItem: HistoryItem = {
        id: Date.now().toString(),
        expression: display,
        result: resultStr,
        timestamp: new Date(),
      };
      setHistory([historyItem, ...history]);
      
      setDisplay(resultStr);
      setExpression(resultStr);
    } catch (error) {
      setDisplay('错误');
      setExpression('');
    }
  };

  const handleClearHistory = () => {
    setHistory([]);
  };

  const handleSelectHistory = (item: HistoryItem) => {
    setDisplay(item.result);
    setExpression(item.result);
    setShowHistory(false);
  };

  const modes = [
    { id: 'standard' as const, label: '标准', icon: Calculator },
    { id: 'scientific' as const, label: '科学', icon: FlaskConical },
    { id: 'programmer' as const, label: '程序员', icon: Binary },
  ];

  return (
    <div className="min-h-screen bg-gradient-to-br from-indigo-100 via-purple-50 to-pink-100 dark:from-gray-950 dark:via-gray-900 dark:to-gray-950 flex items-center justify-center p-4">
      <div className="w-full max-w-2xl bg-white/80 dark:bg-gray-900/80 backdrop-blur-xl rounded-3xl shadow-2xl border border-gray-200/50 dark:border-gray-700/50 overflow-hidden relative">
        {showHistory ? (
          <HistoryPanel
            history={history}
            onClose={() => setShowHistory(false)}
            onClearHistory={handleClearHistory}
            onSelectHistory={handleSelectHistory}
          />
        ) : (
          <>
            {/* 头部导航 */}
            <div className="flex items-center justify-between p-6 border-b border-gray-200/50 dark:border-gray-700/50 bg-white/50 dark:bg-gray-800/50">
              <div className="flex gap-2">
                {modes.map((m) => (
                  <button
                    key={m.id}
                    onClick={() => setMode(m.id)}
                    className={`flex items-center gap-2 px-5 py-2.5 rounded-full font-medium transition-all duration-300 ${
                      mode === m.id
                        ? 'bg-gradient-to-r from-blue-600 to-indigo-600 text-white shadow-lg shadow-blue-500/30 scale-105'
                        : 'bg-gray-100/80 dark:bg-gray-800/80 hover:bg-gray-200 dark:hover:bg-gray-700 hover:scale-105'
                    }`}
                  >
                    <m.icon className="w-4 h-4" />
                    <span className="text-sm">{m.label}</span>
                  </button>
                ))}
              </div>
              
              <button
                onClick={() => setShowHistory(true)}
                className="p-3 hover:bg-gray-100 dark:hover:bg-gray-800 rounded-full transition-all duration-300 hover:scale-110 relative group"
                title="历史记录"
              >
                <History className="w-5 h-5" />
                {history.length > 0 && (
                  <span className="absolute -top-1 -right-1 w-5 h-5 bg-gradient-to-r from-blue-600 to-indigo-600 text-white text-xs rounded-full flex items-center justify-center font-bold shadow-lg">
                    {history.length > 9 ? '9+' : history.length}
                  </span>
                )}
              </button>
            </div>

            {/* 显示屏 */}
            <div className="p-8 bg-gradient-to-br from-gray-50 to-gray-100 dark:from-gray-900 dark:to-gray-950">
              <div className="text-right">
                <div className="text-sm text-gray-500 dark:text-gray-400 h-6 overflow-x-auto font-mono">
                  {expression || ' '}
                </div>
                <div className="text-6xl font-bold mt-3 overflow-x-auto bg-gradient-to-r from-gray-900 to-gray-700 dark:from-white dark:to-gray-300 bg-clip-text text-transparent">
                  {display}
                </div>
              </div>
            </div>

            {/* 计算器按钮区 */}
            <div className="p-8 bg-white/50 dark:bg-gray-900/50">
              {mode === 'standard' && (
                <StandardCalculator
                  display={display}
                  onButtonClick={handleButtonClick}
                  onClear={handleClear}
                  onDelete={handleDelete}
                  onEquals={handleEquals}
                />
              )}
              
              {mode === 'scientific' && (
                <ScientificCalculator
                  display={display}
                  onButtonClick={handleButtonClick}
                  onClear={handleClear}
                  onDelete={handleDelete}
                  onEquals={handleEquals}
                />
              )}
              
              {mode === 'programmer' && (
                <ProgrammerCalculator
                  display={display}
                  onButtonClick={handleButtonClick}
                  onClear={handleClear}
                  onDelete={handleDelete}
                  onEquals={handleEquals}
                />
              )}
            </div>
          </>
        )}
      </div>
    </div>
  );
}