#!/bin/bash

echo "╔════════════════════════════════════════════════════════╗"
echo "║                                                        ║"
echo "║       Redis-Lite Dashboard - Complete Setup           ║"
echo "║                                                        ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""

# Check if running with sudo for installation
if [ "$EUID" -ne 0 ]; then 
    echo "⚠️  This script needs sudo privileges to install Node.js"
    echo ""
    echo "Run with: sudo ./setup.sh"
    echo ""
    exit 1
fi

echo "📦 Step 1: Installing Node.js and npm..."
echo ""

# Update package list
apt update

# Install Node.js and npm
apt install -y nodejs npm

# Check installation
if ! command -v node &> /dev/null; then
    echo "❌ Failed to install Node.js"
    exit 1
fi

if ! command -v npm &> /dev/null; then
    echo "❌ Failed to install npm"
    exit 1
fi

echo ""
echo "✓ Node.js installed: $(node --version)"
echo "✓ npm installed: $(npm --version)"
echo ""

echo "📦 Step 2: Installing backend dependencies..."
cd backend
sudo -u $SUDO_USER npm install
echo "✓ Backend dependencies installed"
echo ""

echo "📦 Step 3: Installing frontend dependencies..."
cd ../frontend
sudo -u $SUDO_USER npm install
echo "✓ Frontend dependencies installed"
echo ""

echo "╔════════════════════════════════════════════════════════╗"
echo "║                                                        ║"
echo "║            ✅ SETUP COMPLETE!                          ║"
echo "║                                                        ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""
echo "🎉 Everything is ready!"
echo ""
echo "📝 To start the dashboard, run these commands in 4 separate terminals:"
echo ""
echo "Terminal 1 (Redis Server):"
echo "  cd ~/Documents/Redis-Lite"
echo "  ./redis-lite"
echo ""
echo "Terminal 2 (Backend):"
echo "  cd ~/Documents/Redis-Lite/web-dashboard/backend"
echo "  npm start"
echo ""
echo "Terminal 3 (Frontend):"
echo "  cd ~/Documents/Redis-Lite/web-dashboard/frontend"
echo "  npm start"
echo ""
echo "Terminal 4 (Worker):"
echo "  cd ~/Documents/Redis-Lite/task-queue"
echo "  ./worker"
echo ""
echo "🌐 Dashboard will open at: http://localhost:3000"
echo ""
