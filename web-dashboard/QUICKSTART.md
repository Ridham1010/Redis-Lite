# 🚀 Quick Start Guide

## ⚡ Installation (One-Time Setup)

```bash
cd web-dashboard
./install.sh
```

## 🏃 Running the Dashboard

### Option 1: Manual (4 Terminals)

**Terminal 1 - Redis Server:**
```bash
cd /home/atharv/Documents/Redis-Lite
./redis-lite
```

**Terminal 2 - Backend API:**
```bash
cd /home/atharv/Documents/Redis-Lite/web-dashboard/backend
npm start
```

**Terminal 3 - Frontend:**
```bash
cd /home/atharv/Documents/Redis-Lite/web-dashboard/frontend
npm start
```

**Terminal 4 - Workers:**
```bash
cd /home/atharv/Documents/Redis-Lite/task-queue
./worker
# Run `./worker` in more terminals to add workers
```

### Option 2: Using tmux (Advanced)

```bash
# Start everything in one command
tmux new-session -d -s redis 'cd ~/Documents/Redis-Lite && ./redis-lite'
tmux split-window -h -t redis 'cd ~/Documents/Redis-Lite/web-dashboard/backend && npm start'
tmux split-window -v -t redis 'cd ~/Documents/Redis-Lite/web-dashboard/frontend && npm start'
tmux split-window -v -t redis 'cd ~/Documents/Redis-Lite/task-queue && ./worker'
tmux attach -t redis
```

## 🌐 Access Dashboard

Open browser to: **http://localhost:3000**

## 🛑 Stopping Everything

Press `Ctrl+C` in each terminal, or:

```bash
# Kill all processes
pkill -f redis-lite
pkill -f "node.*server.js"
pkill -f "react-scripts start"
pkill -f worker
```

## 📊 Dashboard Panels

| Panel | What It Shows |
|-------|---------------|
| **Left (Task Creator)** | Create tasks, select type & priority |
| **Middle (Redis Status)** | All tasks, queues, search |
| **Right (Worker Panel)** | Live worker monitoring |
| **Top Bar** | Connection status, stats |

## 🎯 Common Tasks

### Create a Task
1. Left panel → Select task type
2. Choose priority (Critical/High/Normal/Low)
3. Click "Create Task"

### View Task Status
- Middle panel shows all tasks
- Or search by ID: `task:1000`

### Monitor Workers
- Right panel shows live worker status
- Toggle auto-refresh on/off

### Add More Workers
```bash
# Just run in new terminal
cd task-queue
./worker
```

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| Backend won't start | Make sure Redis-Lite is running first |
| Frontend can't connect | Check backend is on port 5000 |
| No workers showing | Rebuild: `cd task-queue && make` |
| Port in use | `lsof -ti:5000 \| xargs kill -9` |

## 📞 Ports

- **Redis-Lite**: 6379
- **Backend API**: 5000
- **Frontend**: 3000

## 🎨 Features

✅ Create 10 different task types  
✅ 4 priority levels  
✅ Real-time task monitoring  
✅ Live worker status  
✅ Search tasks by ID  
✅ Queue statistics  
✅ Auto-refresh (2-5 seconds)  
✅ Session tracking  
✅ Dark theme UI  

---

**Need help?** Check `web-dashboard/README.md` for detailed documentation.
