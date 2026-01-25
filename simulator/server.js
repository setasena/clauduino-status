const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = 3000;

// Store connected SSE clients
let clients = [];

// Current status
let currentStatus = 'idle';

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
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('OK - Status: COMPLETE (Green LED)');
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
  console.log(`
╔═══════════════════════════════════════════════════════════╗
║                                                           ║
║   🔌 Arduino LED Simulator Server                         ║
║                                                           ║
║   Web UI:  http://localhost:${PORT}                         ║
║                                                           ║
║   API Endpoints:                                          ║
║   ├── GET /red     → Set to Idle (Red LED)               ║
║   ├── GET /yellow  → Set to Processing (Yellow LED)      ║
║   ├── GET /green   → Set to Complete (Green LED)         ║
║   └── GET /status  → Get current status                  ║
║                                                           ║
║   Test with curl:                                         ║
║   $ curl http://localhost:${PORT}/yellow                    ║
║                                                           ║
╚═══════════════════════════════════════════════════════════╝
  `);
});
