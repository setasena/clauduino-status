# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Clauduino Status is an IoT project that creates a physical LED status indicator for Claude Code. It displays operational state through a WiFi-connected ESP8266/ESP32 microcontroller with three LEDs (Red=idle, Yellow=processing with breathing animation, Green=complete).

## Project Structure

```
clauduino-status/
├── firmware/claude_code_led/   # Arduino/ESP firmware
│   └── claude_code_led.ino
├── simulator/                   # Node.js test server
│   ├── server.js
│   └── index.html
├── scripts/                     # Integration scripts
│   └── claude-led.sh
└── docs/                        # Documentation
    ├── QUICK_START.md
    ├── HARDWARE_GUIDE.md
    ├── FIRMWARE_GUIDE.md
    ├── INTEGRATION_GUIDE.md
    ├── API_REFERENCE.md
    └── TROUBLESHOOTING.md
```

## Commands

### Node.js Simulator
```bash
cd simulator && node server.js    # Start test server on port 3000
```
Access UI at http://localhost:3000

### Arduino/ESP Firmware
- Open `firmware/claude_code_led/claude_code_led.ino` in Arduino IDE 2.x
- Select board: ESP8266 NodeMCU or ESP32 DevKit
- Set Serial Monitor to 115200 baud
- Board Manager URLs:
  - ESP8266: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
  - ESP32: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`

### API Testing
```bash
curl http://localhost:3000/yellow    # Set processing state
curl http://localhost:3000/status    # Get current status as JSON
```

## API Endpoints

| Endpoint | Alias | LED State |
|----------|-------|-----------|
| `/red` | `/idle` | Red LED on |
| `/yellow` | `/processing` | Yellow LED breathing |
| `/green` | `/complete` | Green LED on |
| `/status` | - | JSON with status, uptime, IP, RSSI |
| `/events` | - | SSE stream (simulator only) |

## Hardware Pin Mapping

```
ESP8266: Red=D1(GPIO5), Yellow=D2(GPIO4), Green=D3(GPIO0)
ESP32:   Red=GPIO5,     Yellow=GPIO18,    Green=GPIO19
All LEDs through 220ohm resistors to ground
```

## Key Patterns

- Non-blocking main loop handles both client requests and LED animation
- Serial debug output uses emoji indicators for status visibility
- Server.js uses Server-Sent Events to sync all connected clients
- WiFi auto-reconnects with LED cycling animation during connection
- Firmware uses `#ifdef` preprocessor directives for ESP8266/ESP32 compatibility

## Claude Code Integration

**Recommended Setup:** Use hooks in `~/.claude/settings.json` for automatic LED status updates.

**For Simulator (localhost:3000):**
```json
{
  "model": "sonnet",
  "hooks": {
    "UserPromptSubmit": [
      {
        "hooks": [
          {
            "type": "command",
            "command": "curl -s --max-time 1 http://localhost:3000/yellow > /dev/null 2>&1",
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
            "command": "curl -s --max-time 1 http://localhost:3000/green > /dev/null 2>&1 && (sleep 5 && curl -s --max-time 1 http://localhost:3000/red > /dev/null 2>&1) &",
            "timeout": 2
          }
        ]
      }
    ]
  }
}
```

**Important:**
- File location: `~/.claude/settings.json` (NOT `~/.claude-code/settings.json`)
- Restart Claude Code after editing settings for hooks to load
- Use `/hooks` command to verify hooks are registered
- Replace `localhost:3000` with device IP (e.g., `192.168.1.100`) for hardware

## Documentation

See `docs/` folder for detailed guides:
- [Quick Start](docs/QUICK_START.md) - Get running in 10 minutes
- [Hardware Guide](docs/HARDWARE_GUIDE.md) - Shopping list and wiring
- [Firmware Guide](docs/FIRMWARE_GUIDE.md) - Arduino setup and upload
- [Integration Guide](docs/INTEGRATION_GUIDE.md) - Connect to Claude Code
- [API Reference](docs/API_REFERENCE.md) - All endpoints
- [Troubleshooting](docs/TROUBLESHOOTING.md) - Common issues
