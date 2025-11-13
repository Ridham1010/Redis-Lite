# Redis-Lite Web Dashboard

A beautiful, real-time web dashboard for monitoring and managing the Redis-Lite task queue system.

## 🎯 Features

- ✨ **Create Tasks**: Interactive UI to create tasks with 10 predefined types
- 📊 **Redis Status**: View all tasks, queue statistics, and search specific tasks
- 👷 **Worker Monitoring**: Real-time worker status and activity tracking
- 🔄 **Auto-Refresh**: Live updates every 2 seconds
- 🎨 **Dark Theme**: Modern, responsive UI with Tailwind CSS
- 📈 **Session Stats**: Track total tasks, pending, processing, and completed

## 📁 Project Structure

```
web-dashboard/
├── backend/              # Node.js Express API
│   ├── server.js        # Main API server
│   ├── redis-client.js  # Redis client library
│   └── package.json
└── frontend/            # React TypeScript app
    ├── src/
    │   ├── App.tsx           # Main application component
    │   ├── index.tsx         # Entry point
    │   ├── index.css         # Tailwind styles
    │   ├── components/
    │   │   ├── TaskCreator.tsx    # Task creation panel
    │   │   ├── RedisStatus.tsx    # Redis status panel
    │   │   └── WorkerPanel.tsx    # Worker monitoring panel
    │   └── services/
    │       └── api.ts        # API service layer
    ├── public/
    │   └── index.html
    ├── package.json
    ├── tsconfig.json
    ├── tailwind.config.js
    └── postcss.config.js
```

## 🚀 Setup Instructions

### Prerequisites

You need **Node.js** and **npm** installed. If not:

```bash
# Install Node.js and npm
sudo apt update
sudo apt install nodejs npm

# Verify installation
node --version
npm --version
```

### Step 1: Install Backend Dependencies

```bash
cd web-dashboard/backend
npm install
```

### Step 2: Install Frontend Dependencies

```bash
cd ../frontend
npm install
```

## 🏃 Running the Dashboard

You need to run **4 processes** simultaneously (use 4 terminal windows):

### Terminal 1: Redis-Lite Server

```bash
cd /home/atharv/Documents/Redis-Lite
./redis-lite
```

### Terminal 2: Backend API Server

```bash
cd /home/atharv/Documents/Redis-Lite/web-dashboard/backend
npm start
```

Output should show:
```
╔════════════════════════════════════════════╗
║   Redis-Lite Dashboard Backend API        ║
╚════════════════════════════════════════════╝

✓ Server running on http://localhost:5000
✓ Connected to Redis-Lite server
```

### Terminal 3: Frontend React App

```bash
cd /home/atharv/Documents/Redis-Lite/web-dashboard/frontend
npm start
```

This will automatically open your browser at `http://localhost:3000`

### Terminal 4: Workers (Optional, run multiple)

```bash
cd /home/atharv/Documents/Redis-Lite/task-queue
./worker

# In another terminal, run more workers:
./worker
./worker
```

## 🌐 Accessing the Dashboard

Once all processes are running, open your browser to:

**http://localhost:3000**

You should see:
- **Left Panel**: Task Creator (create new tasks)
- **Middle Panel**: Redis Server Status (all tasks and queues)
- **Right Panel**: Worker Status (live worker monitoring)
- **Top Bar**: Connection status and session statistics

## 🎮 How to Use

### Creating Tasks

1. Select a task type from dropdown (send_email, process_payment, etc.)
2. Choose priority: Critical 🔴 | High 🟠 | Normal 🟡 | Low 🟢
3. Click "✨ Create Task"
4. Task is immediately queued and visible in Redis Status panel

### Monitoring Tasks

- **All Tasks List**: Shows every task with ID, type, priority, and status
- **Queue Statistics**: Real-time queue counts for each priority
- **Search**: Enter task ID (e.g., `task:1000`) to view details
- **Auto-Refresh**: Click "🔄 Refresh" or tasks update automatically

### Worker Monitoring

- **Total Workers**: How many workers are running
- **Active**: Workers currently connected
- **Processing**: Workers actively working on tasks
- **Worker Cards**: Show status, current task, and last seen time
- **Auto-Refresh Toggle**: Enable/disable live updates

## 🔧 API Endpoints

The backend provides these endpoints:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/health` | GET | Health check |
| `/api/redis/status` | GET | Check Redis connection |
| `/api/tasks/create` | POST | Create a new task |
| `/api/tasks` | GET | Get all tasks |
| `/api/tasks/:taskId` | GET | Get specific task |
| `/api/queues/stats` | GET | Get queue statistics |
| `/api/workers` | GET | Get all worker status |
| `/api/stats` | GET | Get dashboard stats |
| `/api/tasks/clear-completed` | POST | Clear completed tasks |

## 🛠️ Development

### Backend Development

```bash
cd backend
npm run dev  # Uses nodemon for auto-reload
```

### Frontend Development

```bash
cd frontend
npm start  # Hot-reload enabled by default
```

### Building for Production

```bash
cd frontend
npm run build
```

## 📊 How It Works

### Architecture

```
┌─────────────┐      HTTP      ┌─────────────┐      TCP       ┌─────────────┐
│   React     │ ◄────────────► │   Node.js   │ ◄────────────► │ Redis-Lite  │
│   Frontend  │   (Port 3000)  │   Backend   │   (Port 6379)  │   Server    │
└─────────────┘                └─────────────┘                └─────────────┘
                                  (Port 5000)                         ▲
                                                                      │ TCP
                                                                      │
                                                              ┌───────┴────────┐
                                                              │   C++ Workers  │
                                                              │   (Multiple)   │
                                                              └────────────────┘
```

### Data Flow

1. **User creates task** → React sends POST to `/api/tasks/create`
2. **Backend receives** → Connects to Redis-Lite via TCP
3. **Redis stores task** → Hash for metadata, List for queue
4. **Worker polls** → Gets task from queue, updates status
5. **Dashboard refreshes** → Fetches latest data every 2-5 seconds
6. **UI updates** → Shows real-time status changes

### Worker Tracking

Workers now report their status to Redis:

```cpp
// worker.cpp writes:
worker:1 -> {
  status: "processing",
  current_task: "task:1005",
  started_at: "1699876543",
  last_seen: "1699876550"
}
```

Dashboard reads this and shows live worker activity!

## 🐛 Troubleshooting

### Backend can't connect to Redis

```
✗ Failed to connect to Redis-Lite: connect ECONNREFUSED
```

**Solution**: Make sure `./redis-lite` is running first.

### Frontend can't reach backend

```
Failed to fetch
```

**Solution**: Ensure backend is running on port 5000: `npm start` in backend folder.

### Workers not showing up

**Solution**: 
1. Rebuild workers: `cd task-queue && make`
2. Run workers: `./worker`
3. Check if they appear in dashboard after 2 seconds

### Port already in use

```
Error: listen EADDRINUSE: address already in use :::5000
```

**Solution**: Kill process on that port:
```bash
lsof -ti:5000 | xargs kill -9
```

## 🎨 Customization

### Change Backend Port

Edit `backend/server.js`:
```javascript
const PORT = 5000;  // Change this
```

Also update `frontend/src/services/api.ts`:
```typescript
const API_BASE_URL = 'http://localhost:5000/api';  // Update port
```

### Change Refresh Rate

Edit `frontend/src/components/WorkerPanel.tsx`:
```typescript
const interval = setInterval(fetchWorkers, 2000);  // Change 2000 to desired ms
```

### Add More Task Types

Edit both:
1. `frontend/src/components/TaskCreator.tsx` - Update `TASK_TYPES` array
2. `task-queue/producer.cpp` - Update `TASK_TYPES` vector

## 📝 Notes

- **Session Tracking**: Dashboard tracks current session stats separately from all-time data
- **Worker Cleanup**: Workers remove themselves from registry when shut down (Ctrl+C)
- **Auto-Reconnect**: Backend automatically retries Redis connection every 5 seconds
- **CORS Enabled**: Backend allows requests from any origin (development mode)

## 🚀 Next Steps

Want to enhance the dashboard?

1. **Add Charts**: Use Chart.js for task completion graphs
2. **WebSockets**: Replace polling with Socket.io for instant updates
3. **Task History**: Add timeline view of completed tasks
4. **Worker Controls**: Start/stop workers from UI
5. **Task Filtering**: Filter tasks by status, priority, or type
6. **Export Data**: Download task reports as CSV/JSON

## 📄 License

MIT License - Same as Redis-Lite project

---

**Built with ❤️ using React, TypeScript, Tailwind CSS, Node.js, and C++**
