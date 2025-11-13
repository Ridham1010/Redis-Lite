#!/bin/bash

echo "╔════════════════════════════════════════════════════════╗"
echo "║                                                        ║"
echo "║       Redis-Lite Dashboard - Installation Script      ║"
echo "║                                                        ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""

# Check if Node.js is installed
if ! command -v node &> /dev/null; then
    echo "❌ Node.js is not installed!"
    echo ""
    echo "Please install Node.js first:"
    echo "  sudo apt update"
    echo "  sudo apt install nodejs npm"
    echo ""
    exit 1
fi

echo "✓ Node.js version: $(node --version)"
echo "✓ npm version: $(npm --version)"
echo ""

# Install backend dependencies
echo "📦 Installing backend dependencies..."
cd backend
npm install
if [ $? -ne 0 ]; then
    echo "❌ Failed to install backend dependencies"
    exit 1
fi
echo "✓ Backend dependencies installed"
echo ""

# Install frontend dependencies
echo "📦 Installing frontend dependencies..."
cd ../frontend
npm install
if [ $? -ne 0 ]; then
    echo "❌ Failed to install frontend dependencies"
    exit 1
fi
echo "✓ Frontend dependencies installed"
echo ""

echo "╔════════════════════════════════════════════════════════╗"
echo "║                                                        ║"
echo "║            ✓ Installation Complete!                   ║"
echo "║                                                        ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""
echo "📝 Next Steps:"
echo ""
echo "1. Start Redis-Lite server:"
echo "   cd ../.."
echo "   ./redis-lite"
echo ""
echo "2. Start backend (in new terminal):"
echo "   cd web-dashboard/backend"
echo "   npm start"
echo ""
echo "3. Start frontend (in new terminal):"
echo "   cd web-dashboard/frontend"
echo "   npm start"
echo ""
echo "4. Start workers (in new terminal):"
echo "   cd task-queue"
echo "   ./worker"
echo ""
echo "5. Open browser to: http://localhost:3000"
echo ""
