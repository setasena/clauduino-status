---
name: test-api
description: Test Clauduino Status API endpoints and verify LED functionality. Use when users need to: (1) Verify device is responding, (2) Test all LED states (red/yellow/green/waiting), (3) Check status endpoint, (4) Validate SSE stream (simulator only), (5) Debug API connectivity issues.
---

# API Testing Workflow

Test and verify Clauduino Status API endpoints.

## Target Selection

Determine testing target:

**Simulator (localhost):**
```bash
BASE_URL="http://localhost:3000"
```

**Hardware device:**
```bash
BASE_URL="http://192.168.1.XXX"  # Replace with device IP
```

## Test Sequence

### 1. Check Device Reachability

```bash
curl --max-time 2 $BASE_URL/status
```

**Expected:** JSON response with device info:
```json
{
  "status": "idle",
  "ip": "192.168.1.100",
  "uptime": "00:05:23",
  "rssi": -45
}
```

### 2. Test LED States

Test each endpoint with visual confirmation:

```bash
# Red LED (Idle)
curl $BASE_URL/red
# Verify: Red LED on, others off

# Yellow LED (Processing - breathing animation)
curl $BASE_URL/yellow
# Verify: Yellow LED breathing, others off
# Wait 3-5 seconds to see breathing

# Yellow LED (Waiting - blinking animation)
curl $BASE_URL/waiting
# Verify: Yellow LED blinking fast, others off

# Green LED (Complete)
curl $BASE_URL/green
# Verify: Green LED on, others off
```

### 3. Test Endpoint Aliases

Verify aliases work:

```bash
curl $BASE_URL/idle        # Same as /red
curl $BASE_URL/processing  # Same as /yellow
curl $BASE_URL/prompt      # Same as /waiting
curl $BASE_URL/complete    # Same as /green
```

### 4. Test Status Endpoint

Query device status:

```bash
curl $BASE_URL/status | jq
```

**Expected fields:**
- `status`: Current state (idle/processing/waiting/complete)
- `ip`: Device IP address
- `uptime`: Time since boot
- `rssi`: WiFi signal strength (hardware only)

### 5. Test SSE Stream (Simulator Only)

If testing simulator, verify Server-Sent Events:

```bash
curl -N $BASE_URL/events
```

**Expected:** Stream of events when LED states change.

Open browser to `http://localhost:3000` and test state changes sync across clients.

## Automated Test Script

For comprehensive testing, run:

```bash
#!/bin/bash
BASE_URL="${1:-http://localhost:3000}"

echo "Testing Clauduino Status API at $BASE_URL"
echo

echo "1. Status check..."
curl -s $BASE_URL/status | jq
echo

echo "2. Red (Idle)..."
curl -s $BASE_URL/red
sleep 2

echo "3. Yellow (Processing)..."
curl -s $BASE_URL/yellow
sleep 3

echo "4. Waiting (Blinking)..."
curl -s $BASE_URL/waiting
sleep 2

echo "5. Green (Complete)..."
curl -s $BASE_URL/green
sleep 2

echo "6. Back to Red (Idle)..."
curl -s $BASE_URL/red

echo
echo "Test complete! Verify LED transitions visually."
```

Save as `scripts/test-endpoints.sh` and run:
```bash
chmod +x scripts/test-endpoints.sh
./scripts/test-endpoints.sh http://192.168.1.100
```

## Visual Verification Checklist

During testing, confirm:

- [ ] Only one LED on at a time (except during transitions)
- [ ] Yellow breathing animation smooth (not flickering)
- [ ] Yellow blinking animation rapid (~10Hz)
- [ ] Red LED on at idle/startup
- [ ] Status endpoint returns valid JSON
- [ ] No error messages in serial monitor (hardware)

## Performance Testing

Test response times:

```bash
time curl -s $BASE_URL/yellow > /dev/null
```

**Expected:** <100ms for hardware, <20ms for simulator

## Common Issues

**No response:**
- Device offline or IP changed
- Wrong network (device on different subnet)
- Firewall blocking port 80
- Test with ping first: `ping DEVICE_IP`

**Slow response:**
- WiFi signal weak (check RSSI in /status)
- Network congestion
- Device under load

**LEDs not changing:**
- Hardware wiring issue (see `/setup-hardware` skill)
- GPIO conflicts in firmware
- Check serial monitor for errors

**SSE not working:**
- SSE only available in simulator
- Hardware uses HTTP only (no streaming)

## Troubleshooting Commands

```bash
# Check device is on network
ping -c 3 $DEVICE_IP

# Test port 80 open
nc -zv $DEVICE_IP 80

# Check DNS resolution (if using hostname)
nslookup $DEVICE_HOSTNAME

# View response headers
curl -I $BASE_URL/status

# Test with verbose output
curl -v $BASE_URL/yellow
```

## Next Steps

After successful API testing:
1. Configure Claude Code integration (`/configure-integration` skill)
2. Test hook-triggered LED changes
3. Monitor during actual Claude Code usage

## Reference

API documentation: `docs/API_REFERENCE.md`

For integration testing: `docs/INTEGRATION_GUIDE.md`
