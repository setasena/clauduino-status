# Troubleshooting Guide

Common issues and solutions.

## LED Issues

### LED not lighting up

**Symptoms:** LED stays off when it should be on.

**Solutions:**
1. **Check polarity** - LED longer leg (anode) should connect to GPIO pin, shorter leg to resistor
2. **Verify resistor** - Ensure 220ohm resistor is connected between LED cathode and GND
3. **Test with blink sketch** - Upload basic blink example to verify GPIO works
4. **Check pin assignment** - Verify code uses correct pins (D1, D2, D3 for ESP8266)

**Test Script:**
```cpp
// Paste into Arduino IDE and upload
void setup() {
  pinMode(D1, OUTPUT);  // Red
  pinMode(D2, OUTPUT);  // Yellow
  pinMode(D3, OUTPUT);  // Green
}

void loop() {
  digitalWrite(D1, HIGH); delay(500);
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH); delay(500);
  digitalWrite(D2, LOW);
  digitalWrite(D3, HIGH); delay(500);
  digitalWrite(D3, LOW);
}
```

### LED dim or flickering

**Symptoms:** LED is on but very dim or unstable.

**Solutions:**
1. **Lower resistor value** - Try 150ohm (but never below 100ohm)
2. **Check connections** - Loose breadboard connections cause flickering
3. **Use different LED** - Some LEDs have higher forward voltage
4. **Check power** - Ensure USB provides adequate power (500mA)

### Breathing effect not smooth

**Symptoms:** Yellow LED stutters instead of smooth fade.

**Solutions:**
1. **Verify PWM pin** - D2 (GPIO4) supports PWM on ESP8266
2. **Remove blocking code** - Check `loop()` for any `delay()` calls
3. **Reduce `BREATH_INTERVAL`** - Try lowering from 20 to 10 ms
4. **Check WiFi interference** - Heavy network traffic can interrupt timing

---

## WiFi Issues

### Cannot connect to WiFi

**Symptoms:** LED cycles indefinitely, "Connecting..." dots keep printing.

**Solutions:**
1. **Check credentials** - WiFi name and password are case-sensitive
2. **Use 2.4GHz network** - ESP8266 does not support 5GHz WiFi
3. **Move closer** - Weak signal prevents connection
4. **Check router settings** - Some routers block new devices

**Verify credentials:**
```cpp
// Print for debugging (remove in production)
Serial.println(WIFI_SSID);
Serial.println(WIFI_PASSWORD);
```

### WiFi disconnects frequently

**Symptoms:** Device works then stops responding, Serial shows "WiFi disconnected".

**Solutions:**
1. **Check signal strength** - Move device closer to router
2. **Static IP** - Assign static IP to prevent DHCP issues
3. **Router reboot** - Some routers have connection limits
4. **Power supply** - Unstable power causes WiFi drops

**Add static IP:**
```cpp
// Before WiFi.begin()
IPAddress staticIP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFi.config(staticIP, gateway, subnet);
```

### IP address keeps changing

**Symptoms:** Device works, but IP changes after reboot.

**Solutions:**
1. **DHCP reservation** - Configure router to always assign same IP
2. **Static IP in code** - Set fixed IP as shown above
3. **Use mDNS** - Access via `http://claude-led.local` (future feature)

---

## HTTP Issues

### Cannot reach device from computer

**Symptoms:** `curl` times out or "connection refused".

**Solutions:**
1. **Verify IP** - Check Serial Monitor for actual IP
2. **Same network** - Computer and device must be on same WiFi
3. **Ping first** - `ping 192.168.1.xxx` to test connectivity
4. **Firewall** - Disable firewall temporarily to test
5. **Port 80** - Ensure nothing else uses port 80 on device

### Slow response time

**Symptoms:** LED changes after several seconds delay.

**Solutions:**
1. **WiFi signal** - Move device closer to router
2. **Network congestion** - Try different time of day
3. **Add timeout to curl:**
   ```bash
   curl --max-time 2 http://192.168.1.xxx/yellow
   ```

### CORS errors in browser

**Symptoms:** Browser console shows CORS errors.

**Solutions:**
1. **Device enables CORS** - Check `server.enableCORS(true)` in firmware
2. **Use curl instead** - Browser has stricter security
3. **Simulator works** - Test with Node.js simulator first

---

## Upload Issues

### Board not detected

**Symptoms:** Port menu is empty or grayed out.

**Solutions:**
1. **USB cable** - Use data cable, not charge-only
2. **USB driver** - Install CH340 or CP2102 driver
3. **Different port** - Try all USB ports
4. **Different cable** - Some cables are defective

### Upload fails

**Symptoms:** Error during upload, "espcomm_upload_mem failed".

**Solutions:**
1. **Hold FLASH button** - Press and hold while clicking Upload
2. **Reset timing** - Press RESET right after Upload starts
3. **Lower upload speed** - Tools → Upload Speed → 115200
4. **Close Serial Monitor** - Can't upload while monitor is open

### Compilation errors

**Symptoms:** Red errors in Arduino IDE output.

**Solutions:**
1. **Select correct board** - Tools → Board → NodeMCU 1.0
2. **Install board package** - Board Manager → ESP8266
3. **Only one board defined** - Comment out the other `#define`
4. **Check brackets** - Ensure `{}` are balanced after edits

---

## Simulator Issues

### Server won't start

**Symptoms:** `node server.js` shows error.

**Solutions:**
1. **Node.js version** - Requires Node.js 14+
2. **Port in use** - Another app using port 3000
   ```bash
   # Use different port
   PORT=3001 node server.js
   ```
3. **Correct directory** - Run from `simulator/` folder

### Web UI not updating

**Symptoms:** Browser shows old status, buttons don't work.

**Solutions:**
1. **Hard refresh** - Ctrl+Shift+R (Cmd+Shift+R on Mac)
2. **Check SSE** - Open DevTools → Network → filter "events"
3. **Different browser** - Try Chrome/Firefox

---

## Still Stuck?

1. **Check Serial Monitor** - Most issues show error messages
2. **Simplify** - Test one LED with basic blink sketch
3. **Fresh start** - Re-upload firmware after full power cycle
4. **Hardware swap** - Try different LED, resistor, or jumper wire
