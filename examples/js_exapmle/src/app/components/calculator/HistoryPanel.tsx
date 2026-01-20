import { X, Trash2 } from 'lucide-react';
import { History } from 'lucide-react';

export interface HistoryItem {
  id: string;
  expression: string;
  result: string;
  timestamp: Date;
}

interface HistoryPanelProps {
  history: HistoryItem[];
  onClose: () => void;
  onClearHistory: () => void;
  onSelectHistory: (item: HistoryItem) => void;
}

export function HistoryPanel({
  history,
  onClose,
  onClearHistory,
  onSelectHistory,
}: HistoryPanelProps) {
  return (
    <div className="absolute inset-0 bg-gradient-to-br from-white to-gray-50 dark:from-gray-900 dark:to-gray-950 z-10 flex flex-col text-gray-900 dark:text-white">
      <div className="flex items-center justify-between p-6 border-b border-gray-200/50 dark:border-gray-700/50 bg-white/50 dark:bg-gray-800/50 backdrop-blur-sm">
        <h2 className="text-2xl font-bold bg-gradient-to-r from-blue-600 to-indigo-600 bg-clip-text text-transparent">历史记录</h2>
        <div className="flex gap-2">
          {history.length > 0 && (
            <button
              onClick={onClearHistory}
              className="p-3 hover:bg-red-50 dark:hover:bg-red-900/20 rounded-full transition-all duration-300 hover:scale-110 group"
              title="清空历史"
            >
              <Trash2 className="w-5 h-5 text-gray-600 dark:text-gray-400 group-hover:text-red-600 dark:group-hover:text-red-400 transition-colors" />
            </button>
          )}
          <button
            onClick={onClose}
            className="p-3 hover:bg-gray-100 dark:hover:bg-gray-800 rounded-full transition-all duration-300 hover:scale-110"
          >
            <X className="w-5 h-5" />
          </button>
        </div>
      </div>
      
      <div className="flex-1 overflow-y-auto p-6">
        {history.length === 0 ? (
          <div className="text-center text-gray-400 dark:text-gray-500 mt-16">
            <History className="w-16 h-16 mx-auto mb-4 opacity-30" />
            <p className="text-lg">暂无历史记录</p>
          </div>
        ) : (
          <div className="space-y-3">
            {history.map((item) => (
              <button
                key={item.id}
                onClick={() => onSelectHistory(item)}
                className="w-full text-left p-5 rounded-2xl bg-white dark:bg-gray-800 hover:bg-gradient-to-br hover:from-blue-50 hover:to-indigo-50 dark:hover:from-gray-700 dark:hover:to-gray-800 transition-all duration-300 shadow-md hover:shadow-xl hover:scale-[1.02] border border-gray-200/50 dark:border-gray-700/50"
              >
                <div className="text-sm text-gray-600 dark:text-gray-400 font-mono">
                  {item.expression}
                </div>
                <div className="text-3xl font-bold mt-2 bg-gradient-to-r from-gray-900 to-gray-700 dark:from-white dark:to-gray-300 bg-clip-text text-transparent">
                  = {item.result}
                </div>
                <div className="text-xs text-gray-500 dark:text-gray-500 mt-3 flex items-center gap-1">
                  <span className="w-1.5 h-1.5 bg-blue-600 rounded-full"></span>
                  {item.timestamp.toLocaleString('zh-CN')}
                </div>
              </button>
            ))}
          </div>
        )}
      </div>
    </div>
  );
}