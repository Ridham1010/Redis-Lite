import React, { useState } from 'react';
import { api } from '../services/api';

const TASK_TYPES = [
  'send_email',
  'process_payment',
  'generate_report',
  'resize_image',
  'backup_database',
  'send_notification',
  'compress_video',
  'export_data',
  'update_inventory',
  'cleanup_logs'
];

const PRIORITIES = ['critical', 'high', 'normal', 'low'];

interface TaskCreatorProps {
  onTaskCreated: () => void;
}

const TaskCreator: React.FC<TaskCreatorProps> = ({ onTaskCreated }) => {
  const [taskType, setTaskType] = useState(TASK_TYPES[0]);
  const [priority, setPriority] = useState('normal');
  const [creating, setCreating] = useState(false);
  const [message, setMessage] = useState('');

  const handleCreate = async () => {
    setCreating(true);
    setMessage('');

    try {
      const result = await api.createTask(taskType, priority);
      setMessage(`✓ ${result.message}`);
      onTaskCreated();
      
      setTimeout(() => setMessage(''), 3000);
    } catch (err: any) {
      setMessage(`✗ Error: ${err.message}`);
    } finally {
      setCreating(false);
    }
  };

  const getPriorityColor = (p: string) => {
    switch (p) {
      case 'critical': return 'bg-red-600';
      case 'high': return 'bg-orange-500';
      case 'normal': return 'bg-yellow-500';
      case 'low': return 'bg-green-500';
      default: return 'bg-gray-500';
    }
  };

  return (
    <div className="bg-gray-800 rounded-lg p-6 shadow-lg">
      <h2 className="text-2xl font-bold mb-6 text-white">
        Create Task
      </h2>

      <div className="space-y-4">
        {/* Task Type Selection */}
        <div>
          <label className="block text-sm font-medium text-gray-300 mb-2">
            Task Type
          </label>
          <select
            value={taskType}
            onChange={(e) => setTaskType(e.target.value)}
            className="w-full px-4 py-2 bg-gray-700 text-white rounded-lg border border-gray-600 focus:outline-none focus:ring-2 focus:ring-blue-500"
          >
            {TASK_TYPES.map((type) => (
              <option key={type} value={type}>
                {type}
              </option>
            ))}
          </select>
        </div>

        {/* Priority Selection */}
        <div>
          <label className="block text-sm font-medium text-gray-300 mb-2">
            Priority
          </label>
          <div className="grid grid-cols-2 gap-2">
            {PRIORITIES.map((p) => (
              <button
                key={p}
                onClick={() => setPriority(p)}
                className={`px-4 py-2 rounded-lg font-medium transition-all ${
                  priority === p
                    ? `${getPriorityColor(p)} text-white shadow-lg scale-105`
                    : 'bg-gray-700 text-gray-300 hover:bg-gray-600'
                }`}
              >
                {p.charAt(0).toUpperCase() + p.slice(1)}
              </button>
            ))}
          </div>
        </div>

        {/* Create Button */}
        <button
          onClick={handleCreate}
          disabled={creating}
          className={`w-full py-3 rounded-lg font-bold text-white transition-all ${
            creating
              ? 'bg-gray-600 cursor-not-allowed'
              : 'bg-blue-600 hover:bg-blue-700 active:scale-95'
          }`}
        >
          {creating ? 'Creating...' : 'Create Task'}
        </button>

        {/* Status Message */}
        {message && (
          <div className={`p-3 rounded-lg text-sm ${
            message.startsWith('✓')
              ? 'bg-green-900 text-green-200'
              : 'bg-red-900 text-red-200'
          }`}>
            {message}
          </div>
        )}
      </div>

      {/* Info Box */}
      <div className="mt-6 p-4 bg-gray-900 rounded-lg border border-gray-700">
        <h3 className="text-sm font-semibold text-gray-400 mb-2">Selected Task</h3>
        <div className="text-sm text-gray-300 space-y-1">
          <div><span className="text-gray-500">Type:</span> <span className="font-mono">{taskType}</span></div>
          <div><span className="text-gray-500">Priority:</span> <span className="font-semibold">{priority}</span></div>
        </div>
      </div>
    </div>
  );
};

export default TaskCreator;
