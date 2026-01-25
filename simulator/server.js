const http = require('http');
const fs = require('fs');
const path = require('path');
const { spawn } = require('child_process');

const PORT = 3000;

// Parse command line arguments
const args = process.argv.slice(2);
const noSound = args.includes('--no-sound');

// Auto-detect sound capability
const isMacOS = process.platform === 'darwin';
const soundEnabled = !noSound && isMacOS;

// Store connected SSE clients
let clients = [];

// Current status
let currentStatus = 'idle';

// Play sound helper function
function playSound(soundFile) {
  if (!soundEnabled) {
    return;
  }

  const soundPath = path.join(__dirname, '..', soundFile);

  // Check if sound file exists
  if (!fs.existsSync(soundPath)) {
    console.error(`⚠️  Sound file not found: ${soundPath}`);
    return;
  }

  // Spawn afplay in background (non-blocking)
  const afplay = spawn('afplay', [soundPath], {
    detached: true,
    stdio: 'ignore'
  });

  // Unref so it doesn't keep the process alive
  afplay.unref();

  console.log(`🔊 Playing sound: ${path.basename(soundFile)}`);
}

// Broadcast status to all connected clients
function broadcast(status) {
  currentStatus = status;
  clients.forEach(client => {
    client.res.write(`data: ${JSON.stringify({ status })}\n\n`);
  });
  console.log(`📡 Status changed to: ${status.toUpperCase()}`);
}

const server = http.createServer((req, res) => {
  const url = req.url;

  // CORS headers for all requests
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  // Handle preflight
  if (req.method === 'OPTIONS') {
    res.writeHead(200);
    res.end();
    return;
  }

  // API Endpoints
  if (url === '/red' || url === '/idle') {
    broadcast('idle');
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('OK - Status: IDLE (Red LED)');
    return;
  }

  if (url === '/yellow' || url === '/processing') {
    broadcast('processing');
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('OK - Status: PROCESSING (Yellow LED)');
    return;
  }

  if (url === '/green' || url === '/complete') {
    broadcast('complete');
    playSound('sound/finish.m4a');
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('OK - Status: COMPLETE (Green LED)');
    return;
  }

  if (url === '/waiting' || url === '/prompt') {
    broadcast('waiting');
    playSound('sound/waiting_user_input.m4a');
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('OK - Status: WAITING (Yellow LED - Blinking)');
    return;
  }

  // Status endpoint
  if (url === '/status') {
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ status: currentStatus }));
    return;
  }

  // SSE endpoint for real-time updates
  if (url === '/events') {
    res.writeHead(200, {
      'Content-Type': 'text/event-stream',
      'Cache-Control': 'no-cache',
      'Connection': 'keep-alive'
    });

    // Send current status immediately
    res.write(`data: ${JSON.stringify({ status: currentStatus })}\n\n`);

    // Add client to list
    const clientId = Date.now();
    const client = { id: clientId, res };
    clients.push(client);
    console.log(`✅ Client connected: ${clientId}`);

    // Remove client on disconnect
    req.on('close', () => {
      clients = clients.filter(c => c.id !== clientId);
      console.log(`❌ Client disconnected: ${clientId}`);
    });

    return;
  }

  // Serve static files
  if (url === '/' || url === '/index.html') {
    const filePath = path.join(__dirname, 'index.html');
    fs.readFile(filePath, (err, data) => {
      if (err) {
        res.writeHead(500);
        res.end('Error loading page');
        return;
      }
      res.writeHead(200, { 'Content-Type': 'text/html' });
      res.end(data);
    });
    return;
  }

  // 404 for other routes
  res.writeHead(404);
  res.end('Not Found');
});

server.listen(PORT, () => {
  const soundStatus = soundEnabled ? '✅ Enabled' : '❌ Disabled';
  const soundReason = !isMacOS ? ' (not macOS)' : (noSound ? ' (--no-sound flag)' : '');

  console.log(`
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║   🔌 Arduino LED Simulator Server                         ║
║                                                           ║
║   Web UI:  http://localhost:${PORT}                         ║
║   Sound:   ${soundStatus}${soundReason.padEnd(28 - soundStatus.length)}║
║                                                           ║
║   API Endpoints:                                          ║
║   ├── GET /red     → Set to Idle (Red LED)               ║
║   ├── GET /yellow  → Set to Processing (Yellow LED)      ║
║   ├── GET /waiting → Set to Waiting (Yellow + Sound)     ║
║   ├── GET /green   → Set to Complete (Green + Sound)     ║
║   └── GET /status  → Get current status                  ║
║                                                           ║
║   Test with curl:                                         ║
║   $ curl http://localhost:${PORT}/yellow                    ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
  `);
});
