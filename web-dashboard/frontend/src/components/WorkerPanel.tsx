import React, { useState, useEffect } from 'react';
import { api, Worker } from '../services/api';

const WorkerPanel: React.FC = () => {
  const [workers, setWorkers] = useState<Worker[]>([]);
  const [autoRefresh, setAutoRefresh] = useState(true);

  const fetchWorkers = async () => {
    try {
      const data = await api.getWorkers();
      // Filter to show only active workers
      const activeWorkers = data.filter(w => w.isActive);
      setWorkers(activeWorkers);
    } catch (err) {
      console.error('Error fetching workers:', err);
    }
  };

  useEffect(() => {
    fetchWorkers();
  }, []);

  useEffect(() => {
    if (!autoRefresh) return;

    const interval = setInterval(fetchWorkers, 2000); // Refresh every 2 seconds
    return () => clearInterval(interval);
  }, [autoRefresh]);

  const getWorkerStatusColor = (status: string, isActive: boolean) => {
    if (!isActive) return 'text-gray-500';
    return status === 'processing' ? 'text-yellow-400' : 'text-green-400';
  };

  const formatTimestamp = (timestamp: string) => {
    const date = new Date(parseInt(timestamp) * 1000);
    return date.toLocaleTimeString();
  };

  const getTimeAgo = (timestamp: string) => {
    const now = Math.floor(Date.now() / 1000);
    const diff = now - parseInt(timestamp);
    
    if (diff < 60) return `${diff}s ago`;
    if (diff < 3600) return `${Math.floor(diff / 60)}m ago`;
    return `${Math.floor(diff / 3600)}h ago`;
  };

  return (
    <div className="bg-gray-800 rounded-lg p-6 shadow-lg h-full overflow-hidden flex flex-col">
      <div className="flex items-center justify-between mb-6">
        <h2 className="text-2xl font-bold text-white">
          Worker Status
        </h2>
        <button
          onClick={() => setAutoRefresh(!autoRefresh)}
          className={`px-3 py-1 rounded text-sm font-semibold transition-colors ${
            autoRefresh
              ? 'bg-green-600 text-white'
              : 'bg-gray-700 text-gray-300'
          }`}
        >
          {autoRefresh ? 'Auto' : 'Manual'}
        </button>
      </div>

      {/* Worker Count Summary */}
      <div className="mb-6 grid grid-cols-3 gap-3">
        <div className="bg-blue-900 rounded p-3 text-center">
          <div className="text-blue-300 text-xs">Total Workers</div>
          <div className="text-white font-bold text-2xl">{workers.length}</div>
        </div>
        <div className="bg-green-900 rounded p-3 text-center">
          <div className="text-green-300 text-xs">Active</div>
          <div className="text-white font-bold text-2xl">
            {workers.filter(w => w.isActive).length}
          </div>
        </div>
        <div className="bg-yellow-900 rounded p-3 text-center">
          <div className="text-yellow-300 text-xs">Processing</div>
          <div className="text-white font-bold text-2xl">
            {workers.filter(w => w.status === 'processing' && w.isActive).length}
          </div>
        </div>
      </div>

      {/* Manual Refresh Button */}
      {!autoRefresh && (
        <button
          onClick={fetchWorkers}
          className="mb-4 w-full py-2 bg-gray-700 text-white rounded-lg hover:bg-gray-600 transition-colors"
        >
          Refresh
        </button>
      )}

      {/* Workers List */}
      <div className="flex-1 overflow-y-auto">
        <div className="space-y-3">
          {workers.length === 0 ? (
            <div className="text-center text-gray-500 py-8">
              <div>No workers running.</div>
              <div className="text-sm mt-1">Start workers with: <code className="bg-gray-900 px-2 py-1 rounded">./worker</code></div>
            </div>
          ) : (
            workers.map((worker) => (
              <div
                key={worker.id}
                className={`rounded-lg p-4 border transition-all ${
                  worker.isActive
                    ? worker.status === 'processing'
                      ? 'bg-yellow-900 border-yellow-700'
                      : 'bg-green-900 border-green-700'
                    : 'bg-gray-900 border-gray-700 opacity-60'
                }`}
              >
                <div className="flex items-center justify-between mb-2">
                  <div className="flex items-center gap-2">
                    <div>
                      <div className="font-semibold text-white">{worker.id}</div>
                      <div className="text-xs text-gray-400">
                        Last seen: {getTimeAgo(worker.last_seen)}
                      </div>
                    </div>
                  </div>
                  <div className={`text-sm font-semibold ${getWorkerStatusColor(worker.status, worker.isActive)}`}>
                    {worker.isActive ? (
                      worker.status === 'processing' ? 'PROCESSING' : 'IDLE'
                    ) : (
                      'OFFLINE'
                    )}
                  </div>
                </div>

                {worker.current_task && worker.isActive && (
                  <div className="mt-2 pt-2 border-t border-gray-700">
                    <div className="text-xs text-gray-400">Current Task:</div>
                    <div className="text-sm font-mono text-white">{worker.current_task}</div>
                  </div>
                )}

                {worker.started_at && (
                  <div className="mt-2 text-xs text-gray-500">
                    Started: {formatTimestamp(worker.started_at)}
                  </div>
                )}
              </div>
            ))
          )}
        </div>
      </div>
    </div>
  );
};

export default WorkerPanel;
