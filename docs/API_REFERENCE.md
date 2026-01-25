# API Reference

Complete reference for all HTTP endpoints.

## Base URL

**Hardware device:**
```
http://<DEVICE_IP>:80
```

**Simulator:**
```
http://localhost:3000
```

## Endpoints

### Set Idle State

Sets the red LED on (solid). Indicates Claude is waiting for input.

```http
GET /red
GET /idle
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/plain

OK - Status: IDLE (Red LED)
```

**Example:**
```bash
curl http://192.168.1.100/red
curl http://192.168.1.100/idle
```

---

### Set Processing State

Sets the yellow LED on with breathing animation. Indicates Claude is working.

```http
GET /yellow
GET /processing
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/plain

OK - Status: PROCESSING (Yellow LED)
```

**Example:**
```bash
curl http://192.168.1.100/yellow
curl http://192.168.1.100/processing
```

**Breathing Animation:**
- Fade in: 750ms (0% to 100% brightness)
- Fade out: 750ms (100% to 0% brightness)
- Total cycle: 1500ms

---

### Set Complete State

Sets the green LED on (solid). Indicates task finished successfully.

```http
GET /green
GET /complete
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/plain

OK - Status: COMPLETE (Green LED)
```

**Example:**
```bash
curl http://192.168.1.100/green
curl http://192.168.1.100/complete
```

---

### Get Status

Returns current device status as JSON.

```http
GET /status
```

**Response:**
```json
{
  "status": "processing",
  "uptime": 3600,
  "ip": "192.168.1.100",
  "rssi": -45
}
```

| Field | Type | Description |
|-------|------|-------------|
| status | string | Current state: "idle", "processing", or "complete" |
| uptime | number | Seconds since device boot |
| ip | string | Device IP address |
| rssi | number | WiFi signal strength in dBm (hardware only) |

**Example:**
```bash
curl http://192.168.1.100/status | jq .
```

---

### Server-Sent Events (Simulator Only)

Real-time status updates via SSE stream.

```http
GET /events
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/event-stream
Cache-Control: no-cache
Connection: keep-alive

data: {"status":"idle"}

data: {"status":"processing"}

data: {"status":"complete"}
```

**Example (JavaScript):**
```javascript
const evtSource = new EventSource('http://localhost:3000/events');
evtSource.onmessage = (event) => {
  const data = JSON.parse(event.data);
  console.log('Status:', data.status);
};
```

**Note:** This endpoint is only available in the Node.js simulator, not on the hardware device.

---

### Root Page

Returns a simple HTML control page (hardware device only).

```http
GET /
```

Open in browser to see status and control buttons.

---

## Error Responses

### Not Found

```http
HTTP/1.1 404 Not Found
Content-Type: text/plain

Not Found
```

Returned for any undefined endpoint.

---

## CORS

All endpoints support CORS with the following headers:

```http
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Headers: Content-Type
```

This allows calling the API from any web page.

---

## Quick Reference

| Endpoint | Alias | Action | LED |
|----------|-------|--------|-----|
| `/red` | `/idle` | Set idle state | Red (solid) |
| `/yellow` | `/processing` | Set processing state | Yellow (breathing) |
| `/green` | `/complete` | Set complete state | Green (solid) |
| `/status` | - | Get JSON status | - |
| `/events` | - | SSE stream (simulator) | - |

---

## Testing Sequence

Test a full Claude Code workflow:

```bash
# Start processing
curl http://192.168.1.100/yellow

# Wait (simulating Claude thinking)
sleep 3

# Complete
curl http://192.168.1.100/green

# Wait for user to notice
sleep 2

# Return to idle
curl http://192.168.1.100/red
```
