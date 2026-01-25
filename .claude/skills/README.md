# Clauduino Status Skills

This directory contains Claude Code skills for the Clauduino Status project. These skills provide specialized guidance to AI agents working with this codebase.

## Available Skills

### 1. setup-hardware (`/setup-hardware`)
Guide users through complete hardware setup including pin mapping (ESP8266 vs ESP32), resistor calculations, wiring verification, and initial power-on checks.

**Use when:**
- Assembling hardware for the first time
- Troubleshooting wiring issues
- Verifying component connections
- Choosing correct pins for board type

### 2. flash-firmware (`/flash-firmware`)
Streamline firmware upload process with board detection, Arduino IDE configuration, and serial verification at 115200 baud.

**Use when:**
- Uploading firmware to ESP board
- Setting up Arduino IDE
- Configuring board manager URLs
- Verifying successful firmware upload
- Troubleshooting upload failures

### 3. configure-integration (`/configure-integration`)
Set up Claude Code hooks integration in `~/.claude/settings.json` with correct device IP and optional sound notifications.

**Use when:**
- Connecting Claude Code to device
- Setting up automated LED control
- Configuring hooks for status updates
- Adding sound notifications (macOS)
- Testing hook execution

### 4. test-api (`/test-api`)
Quick API endpoint testing workflow to verify all LED states and check device connectivity.

**Use when:**
- Verifying device is responding
- Testing LED state transitions
- Debugging API connectivity
- Checking status endpoint
- Validating functionality

### 5. add-led-pattern (`/add-led-pattern`)
Add new LED animation patterns with templates for firmware modification and documentation updates.

**Use when:**
- Creating custom LED animations
- Adding new visual patterns (fade, pulse, strobe)
- Extending firmware with new states
- Implementing new API endpoints
- Updating project documentation

### 6. troubleshoot (`/troubleshoot`)
Systematic debugging for WiFi connection, serial monitor, hooks, and network issues.

**Use when:**
- Diagnosing connection problems
- Debugging LED behavior issues
- Investigating hook failures
- Resolving network connectivity
- Serial monitor troubleshooting

## Using Skills

Skills are invoked in Claude Code with the `/` prefix:

```
/setup-hardware
/flash-firmware
/configure-integration
/test-api
/add-led-pattern
/troubleshoot
```

## Workflow

Typical setup workflow:

1. `/setup-hardware` - Assemble and wire components
2. `/flash-firmware` - Upload firmware to board
3. `/test-api` - Verify device functionality
4. `/configure-integration` - Connect Claude Code
5. `/troubleshoot` - If any issues arise
6. `/add-led-pattern` - Extend with custom patterns (optional)

## Skill Structure

Each skill follows this structure:

```
skill-name/
├── SKILL.md (required)
│   ├── YAML frontmatter (name, description)
│   └── Markdown instructions
├── scripts/ (optional)
│   └── Helper scripts
└── references/ (optional)
    └── Additional documentation
```

## Contributing

To add new skills:

1. Create skill directory in `.claude/skills/`
2. Add `SKILL.md` with YAML frontmatter
3. Include clear triggering conditions in description
4. Provide step-by-step instructions
5. Test with actual use cases
6. Update this README

## References

- Project documentation: `docs/`
- Main instructions: `CLAUDE.md`
- Firmware: `firmware/claude_code_led/claude_code_led.ino`
- Test server: `simulator/server.js`
