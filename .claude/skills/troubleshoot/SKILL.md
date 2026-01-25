---
name: troubleshoot
description: "Systematic debugging for Clauduino Status issues. Use when users experience: (1) WiFi connection failures, (2) Device not responding to API calls, (3) LEDs not working or incorrect behavior, (4) Claude Code hooks not triggering, (5) Serial monitor errors, (6) Network connectivity problems."
---

# Troubleshooting Guide

Systematic diagnosis for Clauduino Status issues.

## Diagnostic Workflow

### Step 1: Identify Issue Category

Determine which category the issue falls into:

1. **Hardware** - LEDs not lighting, wiring issues
2. **WiFi** - Cannot connect to network
3. **Network** - Device unreachable via API
4. **Firmware** - Upload failures, serial errors
5. **Integration** - Claude Code hooks not working
6. **Performance** - Slow response, delays, crashes

## Hardware Issues

### LEDs Not Lighting

**Diagnosis:**

1. Check serial monitor at 115200 baud for status messages
2. Verify power: Is board powered via USB?
3. Test API: `curl http://DEVICE_IP/status`

**Common causes:**

```bash
# Check wiring with multimeter
# Expected voltages:
# - GPIO pin HIGH: ~3.3V
# - Ground: 0V
# - LED forward voltage: ~2V (red), ~3V (yellow/green)
```

**Solutions:**

- **No LED at all:** Wrong polarity (swap LED direction)
- **Dim LED:** Missing/wrong resistor value
- **Flickering:** Poor breadboard connection
- **Wrong LED lights:** Incorrect GPIO pins in firmware

### Wiring Verification

Run through checklist:

- [ ] LED long leg connects to GPIO pin
- [ ] LED short leg connects to resistor
- [ ] Resistor connects to ground rail
- [ ] Ground rail connects to ESP GND
- [ ] All connections firm in breadboard
- [ ] No short circuits between LED legs

See `/setup-hardware` skill for correct wiring.

## WiFi Issues

### Cannot Connect to Network

**Check serial monitor output:**

Expected:
```
Connecting to WiFi: YourNetworkName
....✅ WiFi Connected!
📍 IP Address: 192.168.1.100
```

If stuck on dots (`...`), WiFi not connecting.

**Diagnosis commands:**

```bash
# Check WiFi credentials in firmware
grep "WIFI_SSID\|WIFI_PASSWORD" firmware/claude_code_led/claude_code_led.ino
```

**Common causes:**

1. **Wrong credentials** - SSID or password incorrect
2. **5GHz network** - ESP8266/32 only support 2.4GHz
3. **Hidden SSID** - May not work, unhide network
4. **Special characters** - Escape quotes in password
5. **MAC filtering** - Add device MAC to router whitelist
6. **Weak signal** - Move closer to router

**Solutions:**

1. Update credentials in firmware line 30-31:
   ```cpp
   const char* WIFI_SSID = "YourNetworkName";
   const char* WIFI_PASSWORD = "YourPassword";
   ```

2. Verify 2.4GHz network:
   - Check router settings
   - Create separate 2.4GHz SSID if needed

3. Test with hotspot:
   - Create phone hotspot (2.4GHz)
   - Update firmware with hotspot credentials
   - If works, issue is router/network config

### WiFi Disconnecting Randomly

**Check RSSI (signal strength):**

```bash
curl http://DEVICE_IP/status | jq .rssi
```

**Signal quality:**
- `-50 dBm or better`: Excellent
- `-60 to -50`: Good
- `-70 to -60`: Fair
- `-80 to -70`: Weak
- `-80 or worse`: Very weak (frequent disconnects)

**Solutions:**
- Move device closer to router
- Use WiFi extender
- Reduce interference (microwave, Bluetooth)
- Update router firmware

## Network Issues

### Device Not Responding

**Step-by-step diagnosis:**

1. **Verify device on network:**
   ```bash
   ping DEVICE_IP
   ```
   If no response, device offline or IP changed.

2. **Check IP changed:**
   - Open serial monitor
   - Look for IP address in boot message
   - Router may have reassigned IP (DHCP)

3. **Test port 80 accessible:**
   ```bash
   nc -zv DEVICE_IP 80
   ```
   Should show: `Connection to DEVICE_IP port 80 [tcp/http] succeeded!`

4. **Check firewall:**
   ```bash
   # macOS - check if firewall blocking
   /usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate
   ```

5. **Verify same subnet:**
   ```bash
   # Computer IP
   ifconfig | grep "inet " | grep -v 127.0.0.1

   # Device should be on same subnet (e.g., 192.168.1.x)
   ```

**Common causes:**

- **IP changed:** Router DHCP reassignment
- **Different network:** Computer on 5GHz, device on 2.4GHz
- **VPN active:** Routing through VPN blocks local network
- **Firewall:** Blocking port 80

**Solutions:**

1. **Set static IP on router:**
   - Router admin panel → DHCP → Reserve IP for device MAC
   - Prevents IP changes

2. **Use mDNS/Hostname (future enhancement):**
   - Access via `http://clauduino.local` instead of IP
   - Requires mDNS library in firmware

### Slow API Response

**Test response time:**

```bash
time curl -s http://DEVICE_IP/yellow > /dev/null
```

**Expected:** <100ms

**If >500ms:**

1. Check WiFi signal (RSSI)
2. Reduce network load
3. Check for serial monitor slowing device (close it during use)
4. Monitor device resources (memory leaks)

## Firmware Issues

### Upload Fails

**Common errors and solutions:**

**"Port not found":**
```bash
# List available ports
ls /dev/cu.* | grep -i usb

# If empty, driver issue
# ESP8266: Install CH340 driver
# ESP32: Install CP210x driver
```

**"Timeout" during upload:**
- Lower upload speed (115200 → 57600)
- Press BOOT button during upload (ESP32)
- Close serial monitor (holds port)
- Try different USB cable (data not power-only)
- Try different USB port

**"Compilation error":**
- Wrong board selected
- Update board package (Tools → Board Manager)
- Check Arduino IDE version (2.x recommended)

### Serial Monitor Issues

**No output:**

1. Check baud rate: **115200** (must match firmware)
2. Correct port selected
3. Device actually booted (LED cycling during WiFi connect)

**Garbage characters:**
- Wrong baud rate (set to 115200)
- USB cable issue (try different cable)

**Connection refused:**
- Serial monitor already open elsewhere
- Close Arduino IDE serial monitor
- Close other serial terminals (screen, minicom)

## Integration Issues

### Claude Code Hooks Not Triggering

**Diagnosis steps:**

1. **Verify hooks file location:**
   ```bash
   cat ~/.claude/settings.json | jq .hooks
   ```
   Must be `~/.claude/settings.json` NOT `~/.claude-code/settings.json`

2. **Check hooks registered:**
   Run `/hooks` command in Claude Code
   Should list:
   - UserPromptSubmit
   - PermissionRequest
   - Notification
   - Stop

3. **Validate JSON syntax:**
   ```bash
   jq . ~/.claude/settings.json
   ```
   If error, fix JSON formatting

4. **Test hook command manually:**
   ```bash
   curl -s --max-time 1 http://DEVICE_IP/yellow > /dev/null 2>&1
   echo $?  # Should be 0 (success)
   ```

**Common causes:**

- **Wrong file location** - Most common issue
- **Didn't restart Claude Code** - Hooks load on startup
- **JSON syntax error** - Missing comma, quote, bracket
- **Wrong device IP** - IP changed
- **Timeout too short** - Network slow

**Solutions:**

1. Verify file location (use `/configure-integration` skill)
2. Restart Claude Code after changes
3. Validate JSON syntax
4. Update device IP if changed
5. Increase timeout from 1 to 2 seconds

### Hooks Slow Down Claude Code

**If hooks cause delays:**

1. Increase `--max-time` to prevent hanging
2. Add `> /dev/null 2>&1` to silence output
3. Run in background with `&` (for Stop hook)
4. Check network latency to device

## Performance Issues

### Device Crashes/Reboots

**Check serial monitor for:**
- `Watchdog reset` - Loop() taking too long
- `Out of memory` - Memory leak
- `Exception` - Code error

**Solutions:**
- Increase `delay(1)` in loop to 10ms
- Check for memory leaks in custom code
- Reduce serial print frequency

### Animation Stuttering

**Causes:**
- `BREATH_INTERVAL` or `BLINK_INTERVAL` too small
- Network requests blocking animation
- Serial monitor slowing device

**Solutions:**
- Increase interval (20ms → 50ms)
- Close serial monitor during use
- Optimize loop() function

## Quick Diagnostic Script

Run this for comprehensive check:

```bash
#!/bin/bash
DEVICE_IP="${1:-localhost:3000}"

echo "🔍 Clauduino Status Diagnostics"
echo "Testing device: $DEVICE_IP"
echo

echo "1. Network reachability..."
ping -c 3 $DEVICE_IP | tail -1

echo
echo "2. Port 80 accessibility..."
nc -zv $DEVICE_IP 80 2>&1

echo
echo "3. API status endpoint..."
curl -s --max-time 2 http://$DEVICE_IP/status | jq

echo
echo "4. Response time test..."
time curl -s --max-time 2 http://$DEVICE_IP/yellow > /dev/null

echo
echo "5. Claude hooks file..."
ls -la ~/.claude/settings.json

echo
echo "Diagnostics complete!"
```

## Reference Documentation

For detailed troubleshooting:
- Hardware: `docs/HARDWARE_GUIDE.md`
- Firmware: `docs/FIRMWARE_GUIDE.md`
- Integration: `docs/INTEGRATION_GUIDE.md`
- Complete guide: `docs/TROUBLESHOOTING.md`

## Still Having Issues?

If issue persists:

1. Document exact error messages
2. Capture serial monitor output
3. Note device behavior (LED patterns, timing)
4. Check GitHub issues: github.com/YOUR_REPO/issues
5. Provide diagnostic output when asking for help

## Related Skills

- `/setup-hardware` - Fix wiring issues
- `/flash-firmware` - Re-upload firmware
- `/configure-integration` - Fix hook setup
- `/test-api` - Verify API functionality
