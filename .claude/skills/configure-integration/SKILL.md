---
name: configure-integration
description: "Configure Claude Code hooks to control Clauduino Status LED indicator. Use when users need to: (1) Set up ~/.claude/settings.json hooks, (2) Configure device IP address, (3) Add sound notifications (macOS), (4) Test hook execution with /hooks command, (5) Troubleshoot hook triggers."
---

# Claude Code Integration Setup

Configure Claude Code to automatically control Clauduino Status LEDs.

## Prerequisites

- Device IP address (from serial monitor after firmware flash)
- Claude Code CLI installed
- File location: `~/.claude/settings.json` (NOT `~/.claude-code/settings.json`)

## Configuration Workflow

### 1. Get Device IP

From firmware serial monitor or test:
```bash
# For simulator (testing)
DEVICE_IP="localhost:3000"

# For hardware device
DEVICE_IP="192.168.1.XXX"  # Replace with actual IP
```

### 2. Determine Platform

Check if macOS for optional sound integration:
```bash
uname
```

If Darwin (macOS), can add sound notifications.

### 3. Generate Settings Configuration

Create or update `~/.claude/settings.json`:

**Basic Configuration (LED only):**

```json
{
  "model": "sonnet",
  "hooks": {
    "UserPromptSubmit": [
      {
        "hooks": [
          {
            "type": "command",
            "command": "curl -s --max-time 1 http://DEVICE_IP/yellow > /dev/null 2>&1",
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
            "command": "curl -s --max-time 1 http://DEVICE_IP/waiting > /dev/null 2>&1",
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
            "command": "curl -s --max-time 1 http://DEVICE_IP/waiting > /dev/null 2>&1",
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
            "command": "curl -s --max-time 1 http://DEVICE_IP/green > /dev/null 2>&1 && (sleep 5 && curl -s --max-time 1 http://DEVICE_IP/red > /dev/null 2>&1) &",
            "timeout": 2
          }
        ]
      }
    ]
  }
}
```

Replace `DEVICE_IP` with actual device IP address.

### 4. Add Sound Integration (macOS Only)

If macOS and user wants sound notifications, add sound hooks alongside LED control.

**With Sound (macOS):**

Add to each hook array:
```json
{
  "type": "command",
  "command": "afplay /path/to/clauduino-status/sound/waiting_user_input.m4a",
  "timeout": 2
}
```

For complete example with sound, see `docs/SOUND_INTEGRATION.md`.

### 5. Write Configuration File

Read existing `~/.claude/settings.json` first (if exists) to preserve other settings, then merge hooks configuration.

```bash
# Backup existing config
cp ~/.claude/settings.json ~/.claude/settings.json.backup 2>/dev/null || true

# Write new config (use Write tool with merged content)
```

### 6. Restart Claude Code

**IMPORTANT:** Hooks only load on Claude Code startup.

Tell user:
```
Restart Claude Code for hooks to take effect.
```

### 7. Test Hooks

After restart, verify hooks registered:
```
/hooks
```

Should show registered hooks for:
- UserPromptSubmit
- PermissionRequest
- Notification
- Stop

## Hook Behavior

| Claude Event | LED State | Visual |
|-------------|-----------|---------|
| User sends prompt | Yellow breathing | Processing |
| Waiting for permission | Yellow blinking | Waiting for input |
| Waiting for user response | Yellow blinking | Waiting for input |
| Task complete/stopped | Green → Red | Complete, then idle |

## Verification Steps

1. **Check file location:** `cat ~/.claude/settings.json | head`
2. **Test device reachable:** `curl http://DEVICE_IP/status`
3. **Restart Claude Code**
4. **Run `/hooks`** command
5. **Send test prompt** - Yellow LED should light/breathe

## Common Issues

**Hooks not loading:**
- Wrong file location (must be `~/.claude/settings.json`)
- JSON syntax error (validate with `jq . ~/.claude/settings.json`)
- Didn't restart Claude Code
- Use `/hooks` to verify registration

**LED not responding:**
- Device IP changed (router DHCP reassignment)
- Network connectivity issue
- Curl timeout too short (increase from 1 to 2 seconds)
- Test manually: `curl http://DEVICE_IP/yellow`

**Sound not playing (macOS):**
- File path incorrect
- Audio file missing
- Volume muted
- Test manually: `afplay sound/waiting_user_input.m4a`

## Configuration Script

For automated setup, use `scripts/configure.sh` if available:

```bash
scripts/configure.sh --device-ip 192.168.1.100 --sound
```

## Next Steps

After configuration:
1. Restart Claude Code
2. Test with actual usage
3. Adjust timeouts if needed
4. See `/troubleshoot` skill if issues persist

## Reference

- Full configuration: `docs/INTEGRATION_GUIDE.md`
- Sound setup: `docs/SOUND_INTEGRATION.md`
- Hook syntax: Claude Code documentation
