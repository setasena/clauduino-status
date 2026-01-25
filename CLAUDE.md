# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Clauduino Status is an IoT project that creates a physical LED status indicator for Claude Code. It displays operational state through a WiFi-connected ESP8266/ESP32 microcontroller with three LEDs (Red=idle, Yellow=processing with breathing animation/waiting with blinking animation, Green=complete). Optional sound notifications provide audio feedback on macOS.

## Project Structure

```
clauduino-status/
├── firmware/claude_code_led/   # Arduino/ESP firmware
│   └── claude_code_led.ino
├── simulator/                   # Node.js test server
│   ├── server.js
│   └── index.html
├── sound/                       # Audio notification files (macOS)
│   ├── waiting_user_input.m4a
│   └── finish.m4a
├── scripts/                     # Integration scripts
│   ├── claude-led.sh
│   └── play-sound.sh
└── docs/                        # Documentation
    ├── QUICK_START.md
    ├── HARDWARE_GUIDE.md
    ├── FIRMWARE_GUIDE.md
    ├── INTEGRATION_GUIDE.md
    ├── SOUND_INTEGRATION.md
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
| `/waiting` | `/prompt` | Yellow LED blinking |
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

**Sound Notifications (macOS):** The simulator automatically plays sounds when `/waiting` (waiting for user input) and `/green` (complete) endpoints are called. No additional configuration needed! See [Sound Integration Guide](docs/SOUND_INTEGRATION.md) for details.

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
    "PermissionRequest": [
      {
        "matcher": "",
        "hooks": [
          {
            "type": "command",
            "command": "curl -s --max-time 1 http://localhost:3000/waiting > /dev/null 2>&1",
            "timeout": 2
          }
        ]
      }
    ],
    "Notification": [
      {
        "matcher": "idle_prompt",
        "hooks": [
          {
            "type": "command",
            "command": "curl -s --max-time 1 http://localhost:3000/waiting > /dev/null 2>&1",
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

## Claude Code Skills

This project includes specialized skills to guide AI agents through common tasks:

- `/setup-hardware` - Hardware assembly and wiring guidance
- `/flash-firmware` - Firmware upload and Arduino IDE configuration
- `/test-api` - API endpoint testing and verification
- `/configure-integration` - Claude Code hooks setup
- `/add-led-pattern` - Create custom LED animations
- `/troubleshoot` - Systematic debugging for all issues

Skills are located in `.claude/skills/` directory. See [Skills README](.claude/skills/README.md) for details.

## Documentation

See `docs/` folder for detailed guides:
- [Quick Start](docs/QUICK_START.md) - Get running in 10 minutes
- [Hardware Guide](docs/HARDWARE_GUIDE.md) - Shopping list and wiring
- [Firmware Guide](docs/FIRMWARE_GUIDE.md) - Arduino setup and upload
- [Integration Guide](docs/INTEGRATION_GUIDE.md) - Connect to Claude Code
- [Sound Integration](docs/SOUND_INTEGRATION.md) - Add audio notifications (macOS)
- [API Reference](docs/API_REFERENCE.md) - All endpoints
- [Troubleshooting](docs/TROUBLESHOOTING.md) - Common issues
