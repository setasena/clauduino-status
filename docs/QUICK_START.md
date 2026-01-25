# Quick Start Guide

Get your Claude Code LED indicator working in 10 minutes.

## Prerequisites

- [ ] ESP8266 NodeMCU board (or ESP32)
- [ ] 3 LEDs (Red, Yellow, Green)
- [ ] 3x 220ohm resistors
- [ ] Breadboard + jumper wires
- [ ] Arduino IDE 2.x installed
- [ ] USB cable (data capable, not charge-only)
- [ ] WiFi network (2.4GHz)

## Step 1: Wire the Circuit

```
ESP8266 Pin    Component
───────────────────────────────────────
D1 (GPIO5)  →  Red LED (+)    → 220ohm → GND
D2 (GPIO4)  →  Yellow LED (+) → 220ohm → GND
D3 (GPIO0)  →  Green LED (+)  → 220ohm → GND
```

**LED Polarity:**
- Long leg (anode) → GPIO pin
- Short leg (cathode) → resistor → GND

## Step 2: Configure Arduino IDE

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to "Additional Board Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Go to **Tools → Board → Board Manager**
5. Search "ESP8266" and install
6. Select **Tools → Board → ESP8266 Boards → NodeMCU 1.0**

## Step 3: Flash Firmware

1. Open `firmware/claude_code_led/claude_code_led.ino`
2. Edit WiFi credentials (lines 28-29):
   ```cpp
   const char* WIFI_SSID = "YourWiFiName";
   const char* WIFI_PASSWORD = "YourWiFiPassword";
   ```
3. Connect board via USB
4. Select correct port: **Tools → Port**
5. Click **Upload** (arrow button)

## Step 4: Get Device IP

1. Open **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. Press reset button on board
4. Note the IP address displayed:
   ```
   IP Address: 192.168.1.xxx
   ```

## Step 5: Test It

```bash
# Test with curl
curl http://192.168.1.xxx/yellow   # LED should breathe
curl http://192.168.1.xxx/green    # Green LED on
curl http://192.168.1.xxx/red      # Red LED on (idle)
```

## Step 6: Connect to Claude Code

Create `~/.claude/settings.json`:

```bash
mkdir -p ~/.claude
nano ~/.claude/settings.json
```

Add this configuration (replace `192.168.1.xxx` with your device IP):

```json
{
  "model": "sonnet",
  "hooks": {
    "UserPromptSubmit": [
      {
        "hooks": [
          {
            "type": "command",
            "command": "curl -s --max-time 1 http://192.168.1.xxx/yellow > /dev/null 2>&1",
            "timeout": 2
          }
        ]
      }
    ],
    "Stop": [
      {
        "hooks": [
          {
            "type": "command",
            "command": "curl -s --max-time 1 http://192.168.1.xxx/green > /dev/null 2>&1 && (sleep 5 && curl -s --max-time 1 http://192.168.1.xxx/red > /dev/null 2>&1) &",
            "timeout": 2
          }
        ]
      }
    ]
  }
}
```

Save and restart Claude Code for hooks to take effect.

## Done!

Now when you run `claude`, your LED will:
1. Turn **yellow** (breathing) while processing
2. Turn **green** when complete
3. Return to **red** (idle) after 5 seconds

## Next Steps

- [Hardware Guide](HARDWARE_GUIDE.md) - Detailed wiring and component options
- [Integration Guide](INTEGRATION_GUIDE.md) - More connection methods
- [Troubleshooting](TROUBLESHOOTING.md) - Common issues
