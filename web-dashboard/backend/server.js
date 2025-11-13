const express = require('express');
const cors = require('cors');
const RedisClient = require('./redis-client');

const app = express();
const PORT = 5000;

// Middleware
app.use(cors());
app.use(express.json());

// Redis client instance
let redisClient = new RedisClient('127.0.0.1', 6379);
let isConnected = false;

// Connect to Redis on startup
async function connectRedis() {
  try {
    await redisClient.connect();
    isConnected = true;
    console.log('✓ Connected to Redis-Lite server');
  } catch (err) {
    console.error('✗ Failed to connect to Redis-Lite:', err.message);
    isConnected = false;
    // Retry connection after 5 seconds
    setTimeout(connectRedis, 5000);
  }
}

connectRedis();

// ============ API ENDPOINTS ============

// Health check
app.get('/api/health', (req, res) => {
  res.json({ 
    status: 'ok',
    redisConnected: isConnected 
  });
});

// Check Redis connection
app.get('/api/redis/status', async (req, res) => {
  try {
    const pong = await redisClient.ping();
    res.json({ connected: pong });
  } catch (err) {
    res.json({ connected: false });
  }
});

// Create a new task
app.post('/api/tasks/create', async (req, res) => {
  try {
    const { taskType, priority } = req.body;
    
    if (!taskType || !priority) {
      return res.status(400).json({ error: 'Missing taskType or priority' });
    }

    // Generate task ID
    const timestamp = Date.now();
    const taskId = `task:${timestamp}`;
    
    // Store task metadata
    await redisClient.hset(taskId, 'type', taskType);
    await redisClient.hset(taskId, 'priority', priority);
    await redisClient.hset(taskId, 'status', 'pending');
    await redisClient.hset(taskId, 'created_at', Math.floor(timestamp / 1000).toString());
    
    // Add to appropriate queue
    const queueName = `queue:${priority}`;
    if (priority === 'critical' || priority === 'high') {
      await redisClient.lpush(queueName, taskId);
    } else {
      await redisClient.rpush(queueName, taskId);
    }
    
    res.json({ 
      success: true, 
      taskId,
      message: `Task ${taskId} created and queued in ${queueName}` 
    });
  } catch (err) {
    console.error('Error creating task:', err);
    res.status(500).json({ error: err.message });
  }
});

// Get all tasks
app.get('/api/tasks', async (req, res) => {
  try {
    const taskKeys = await redisClient.keys('task:*');
    
    if (!taskKeys || taskKeys.length === 0) {
      return res.json([]);
    }
    
    const tasks = [];
    for (const taskId of taskKeys) {
      const taskData = await redisClient.hgetall(taskId);
      if (taskData && Object.keys(taskData).length > 0) {
        tasks.push({
          id: taskId,
          ...taskData
        });
      }
    }
    
    // Sort by created_at descending
    tasks.sort((a, b) => (b.created_at || 0) - (a.created_at || 0));
    
    res.json(tasks);
  } catch (err) {
    console.error('Error fetching tasks:', err);
    res.status(500).json({ error: err.message });
  }
});

// Get specific task
app.get('/api/tasks/:taskId', async (req, res) => {
  try {
    const { taskId } = req.params;
    const taskData = await redisClient.hgetall(taskId);
    
    if (!taskData || Object.keys(taskData).length === 0) {
      return res.status(404).json({ error: 'Task not found' });
    }
    
    res.json({
      id: taskId,
      ...taskData
    });
  } catch (err) {
    console.error('Error fetching task:', err);
    res.status(500).json({ error: err.message });
  }
});

// Get queue statistics
app.get('/api/queues/stats', async (req, res) => {
  try {
    const critical = await redisClient.llen('queue:critical');
    const high = await redisClient.llen('queue:high');
    const normal = await redisClient.llen('queue:normal');
    const low = await redisClient.llen('queue:low');
    const completed = await redisClient.llen('tasks:completed');
    
    const stats = {
      critical: typeof critical === 'number' ? critical : 0,
      high: typeof high === 'number' ? high : 0,
      normal: typeof normal === 'number' ? normal : 0,
      low: typeof low === 'number' ? low : 0,
      completed: typeof completed === 'number' ? completed : 0
    };
    
    console.log('Queue stats:', stats);
    res.json(stats);
  } catch (err) {
    console.error('Error fetching queue stats:', err);
    res.status(500).json({ error: err.message });
  }
});

// Get worker status
app.get('/api/workers', async (req, res) => {
  try {
    const workerKeys = await redisClient.keys('worker:*');
    
    if (!workerKeys || workerKeys.length === 0) {
      return res.json([]);
    }
    
    const workers = [];
    const now = Math.floor(Date.now() / 1000);
    
    for (const workerKey of workerKeys) {
      const workerData = await redisClient.hgetall(workerKey);
      if (workerData && Object.keys(workerData).length > 0) {
        const lastSeen = parseInt(workerData.last_seen || 0);
        const isActive = (now - lastSeen) < 10; // Consider active if seen in last 10 seconds
        
        workers.push({
          id: workerKey,
          ...workerData,
          isActive
        });
      }
    }
    
    // Sort by worker ID
    workers.sort((a, b) => a.id.localeCompare(b.id));
    
    res.json(workers);
  } catch (err) {
    console.error('Error fetching workers:', err);
    res.status(500).json({ error: err.message });
  }
});

// Get dashboard statistics
app.get('/api/stats', async (req, res) => {
  try {
    const taskKeys = await redisClient.keys('task:*');
    const tasks = taskKeys || [];
    
    let pending = 0, processing = 0, completed = 0;
    
    for (const taskId of tasks) {
      const status = await redisClient.hget(taskId, 'status');
      if (status === 'pending') pending++;
      else if (status === 'processing') processing++;
      else if (status === 'completed') completed++;
    }
    
    res.json({
      totalTasks: tasks.length,
      pending,
      processing,
      completed
    });
  } catch (err) {
    console.error('Error fetching stats:', err);
    res.status(500).json({ error: err.message });
  }
});

// Clear completed tasks (for new session)
app.post('/api/tasks/clear-completed', async (req, res) => {
  try {
    await redisClient.del('tasks:completed');
    res.json({ success: true, message: 'Completed tasks cleared' });
  } catch (err) {
    console.error('Error clearing completed tasks:', err);
    res.status(500).json({ error: err.message });
  }
});

// Start server
app.listen(PORT, () => {
  console.log('╔════════════════════════════════════════════╗');
  console.log('║                                            ║');
  console.log('║   Redis-Lite Dashboard Backend API        ║');
  console.log('║                                            ║');
  console.log('╚════════════════════════════════════════════╝');
  console.log(`\n✓ Server running on http://localhost:${PORT}`);
  console.log(`✓ API endpoints available at /api/*\n`);
});

// Graceful shutdown
process.on('SIGINT', () => {
  console.log('\n\nShutting down gracefully...');
  redisClient.disconnect();
  process.exit(0);
});
