# Clauduino Status

A physical LED status indicator for Claude Code. See at a glance whether Claude is idle, processing, or complete.

```
┌──────────────┐    HTTP     ┌──────────────────────────┐
│  Claude Code │ ──────────▶ │   ESP8266/ESP32 Board    │
│    (CLI)     │   /yellow   │   ┌───┐ ┌───┐ ┌───┐     │
└──────────────┘   /green    │   │ R │ │ Y │ │ G │     │
                   /red      │   └───┘ └───┘ └───┘     │
                             └──────────────────────────┘
```

**LED States:**
- **Red** = Idle (waiting for input)
- **Yellow (Breathing)** = Processing (Claude is thinking)
- **Yellow (Blinking)** = Waiting (Claude needs user decision)
- **Green** = Complete (task finished)

## Quick Links

| I want to... | Go to |
|--------------|-------|
| Get started fast | [Quick Start Guide](docs/QUICK_START.md) |
| Buy components | [Hardware Guide](docs/HARDWARE_GUIDE.md) |
| Flash the firmware | [Firmware Guide](docs/FIRMWARE_GUIDE.md) |
| Connect to Claude Code | [Integration Guide](docs/INTEGRATION_GUIDE.md) |
| See API endpoints | [API Reference](docs/API_REFERENCE.md) |
| Fix problems | [Troubleshooting](docs/TROUBLESHOOTING.md) |

## Try the Simulator

No hardware? Test with the Node.js simulator:

```bash
cd simulator
node server.js
```

Open http://localhost:3000 to see the virtual LEDs.

Test the API:
```bash
curl http://localhost:3000/yellow    # Start processing (breathing)
curl http://localhost:3000/waiting   # Waiting for input (blinking)
curl http://localhost:3000/green     # Complete
curl http://localhost:3000/red       # Back to idle
```

## Requirements

**Hardware (~$10-25 USD):**
- ESP8266 NodeMCU or ESP32 DevKit
- 3 LEDs (Red, Yellow, Green)
- 3x 220ohm resistors
- Breadboard + jumper wires

**Software:**
- Arduino IDE 2.x (or PlatformIO)
- Node.js (for simulator only)

## Project Structure

```
clauduino-status/
├── firmware/claude_code_led/   # Arduino/ESP firmware
├── simulator/                   # Node.js test server + web UI
├── scripts/                     # Ready-to-use wrapper scripts
└── docs/                        # Documentation
```

## How It Works

1. Claude Code hooks trigger HTTP requests to device when you interact with Claude
2. Device changes LED state based on endpoint called (`/yellow`, `/green`, `/red`)
3. Yellow LED breathes while Claude is thinking (PWM animation)
4. Green LED lights when Claude finishes responding
5. Automatically returns to idle (red) after 5 seconds

**Integration Methods:**
- **Hooks** (recommended) - Automatic integration via `~/.claude/settings.json`
- **Wrapper Script** - For non-interactive commands
- **Shell Alias** - Alternative to hooks

## License

MIT
