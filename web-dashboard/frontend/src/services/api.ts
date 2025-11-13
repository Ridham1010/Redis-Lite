const API_BASE_URL = 'http://localhost:5000/api';

export interface Task {
  id: string;
  type: string;
  priority: string;
  status: string;
  created_at: string;
  worker_id?: string;
  completed_at?: string;
}

export interface Worker {
  id: string;
  status: string;
  current_task: string;
  started_at: string;
  last_seen: string;
  isActive: boolean;
}

export interface QueueStats {
  critical: number;
  high: number;
  normal: number;
  low: number;
  completed: number;
}

export interface DashboardStats {
  totalTasks: number;
  pending: number;
  processing: number;
  completed: number;
}

export const api = {
  // Check health
  async checkHealth(): Promise<{ status: string; redisConnected: boolean }> {
    const res = await fetch(`${API_BASE_URL}/health`);
    return res.json();
  },

  // Check Redis connection
  async checkRedisStatus(): Promise<{ connected: boolean }> {
    const res = await fetch(`${API_BASE_URL}/redis/status`);
    return res.json();
  },

  // Create task
  async createTask(taskType: string, priority: string): Promise<{ success: boolean; taskId: string; message: string }> {
    const res = await fetch(`${API_BASE_URL}/tasks/create`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ taskType, priority })
    });
    return res.json();
  },

  // Get all tasks
  async getTasks(): Promise<Task[]> {
    const res = await fetch(`${API_BASE_URL}/tasks`);
    return res.json();
  },

  // Get specific task
  async getTask(taskId: string): Promise<Task> {
    const res = await fetch(`${API_BASE_URL}/tasks/${taskId}`);
    return res.json();
  },

  // Get queue stats
  async getQueueStats(): Promise<QueueStats> {
    const res = await fetch(`${API_BASE_URL}/queues/stats`);
    return res.json();
  },

  // Get workers
  async getWorkers(): Promise<Worker[]> {
    const res = await fetch(`${API_BASE_URL}/workers`);
    return res.json();
  },

  // Get dashboard stats
  async getDashboardStats(): Promise<DashboardStats> {
    const res = await fetch(`${API_BASE_URL}/stats`);
    return res.json();
  },

  // Clear completed tasks
  async clearCompletedTasks(): Promise<{ success: boolean; message: string }> {
    const res = await fetch(`${API_BASE_URL}/tasks/clear-completed`, {
      method: 'POST'
    });
    return res.json();
  }
};
