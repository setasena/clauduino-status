# Sound Integration Guide

This guide shows how to add sound notifications to your Clauduino Status setup.

## Overview

The sound folder contains two audio files:
- `waiting_user_input.m4a` - Plays when Claude Code is waiting for your input
- `finish.m4a` - Plays when Claude Code completes a task

## Prerequisites

- macOS (uses `afplay` command)
- Claude Code installed
- Clauduino Status project files

## Setup Instructions

### Option 1: Using the Helper Script (Recommended)

The `scripts/play-sound.sh` script makes it easy to play sounds from hooks.

**Update your `~/.claude/settings.json` to include sound + LED control:**

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
          },
          {
            "type": "command",
            "command": "/Users/setasenaramadanie/Documents/personal/clauduino-status/scripts/play-sound.sh waiting",
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
          },
          {
            "type": "command",
            "command": "/Users/setasenaramadanie/Documents/personal/clauduino-status/scripts/play-sound.sh waiting",
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
          },
          {
            "type": "command",
            "command": "/Users/setasenaramadanie/Documents/personal/clauduino-status/scripts/play-sound.sh finish",
            "timeout": 2
          }
        ]
      }
    ]
  }
}
```

**Important:** Replace the full path `/Users/setasenaramadanie/Documents/personal/clauduino-status/scripts/play-sound.sh` with the actual path to your project.

### Option 2: Direct afplay Commands

You can also call `afplay` directly in your hooks:

```json
{
  "type": "command",
  "command": "afplay /Users/setasenaramadanie/Documents/personal/clauduino-status/sound/waiting_user_input.m4a &",
  "timeout": 2
}
```

## Testing

Test the sound playback:

```bash
# Test waiting sound
./scripts/play-sound.sh waiting

# Test finish sound
./scripts/play-sound.sh finish
```

Or test directly:

```bash
afplay sound/waiting_user_input.m4a
afplay sound/finish.m4a
```

## Activation

1. Save your `~/.claude/settings.json` file
2. **Restart Claude Code** for hooks to take effect
3. Verify hooks are loaded with `/hooks` command
4. Make sure the simulator is running: `cd simulator && node server.js`

## Customization

### Using Your Own Sounds

Replace the `.m4a` files in the `sound/` folder with your own audio files. Supported formats:
- .m4a (AAC)
- .mp3
- .wav
- .aiff

### Adjusting Volume

The volume is controlled by your system volume. Alternatively, modify the script to use volume control:

```bash
afplay -v 0.5 "$SOUND_DIR/waiting_user_input.m4a" &  # 50% volume
```

## Troubleshooting

### Sound doesn't play
- Verify file paths are absolute paths
- Check that `afplay` works: `which afplay`
- Ensure sound files exist: `ls -la sound/`
- Check system volume isn't muted

### Hooks not triggering
- Verify settings file location: `~/.claude/settings.json` (NOT `~/.claude-code/`)
- Restart Claude Code after editing settings
- Run `/hooks` in Claude Code to see registered hooks
- Check for JSON syntax errors in settings.json

### Sound plays but LEDs don't work
- Verify simulator is running: `curl http://localhost:3000/status`
- Check if using hardware device IP instead of localhost
- Ensure curl commands have correct timeout values

## Hardware Integration

If you're using the physical ESP8266/ESP32 device instead of the simulator, replace `localhost:3000` with your device's IP address:

```json
{
  "type": "command",
  "command": "curl -s --max-time 1 http://192.168.1.100/waiting > /dev/null 2>&1",
  "timeout": 2
}
```

Keep the sound script commands unchanged - they run locally on your Mac.
