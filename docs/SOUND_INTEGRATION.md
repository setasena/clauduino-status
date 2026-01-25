# Sound Integration Guide

This guide explains the built-in sound notification feature in Clauduino Status.

## Overview

The Node.js simulator now includes **built-in sound notifications** that play automatically when certain endpoints are called:
- `waiting_user_input.m4a` - Plays when `/waiting` or `/prompt` endpoint is called (Claude waiting for your input)
- `finish.m4a` - Plays when `/green` or `/complete` endpoint is called (Claude completes a task)

**No additional hook configuration needed!** Just call the endpoint with a single curl command, and the server handles both LED control and sound playback.

## Prerequisites

- macOS (uses `afplay` command)
- Node.js simulator running (`cd simulator && node server.js`)
- Sound files in `sound/` directory

## How It Works

When you start the simulator with `node server.js`, sound is **automatically enabled on macOS**. The server detects your platform and enables sound if you're on macOS.

When you call endpoints like `/waiting` or `/green` via curl (from Claude Code hooks), the server:
1. Updates the LED status (broadcasts to web UI)
2. Plays the corresponding sound file using `afplay`
3. Returns immediately (sound plays in background)

**Configuration:**
- Enabled by default on macOS
- Disabled by default on other platforms
- Can be disabled with `--no-sound` flag: `node server.js --no-sound`
- No environment variables or additional setup required

## Simplified Hooks Configuration

Your `~/.claude/settings.json` only needs **one curl command per hook**:

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

**Benefits:**
- Simple: One curl command instead of two
- No paths: No need to configure absolute paths to scripts
- Automatic: Sound plays automatically when appropriate endpoints are called

## Alternative: Using the Helper Script (Legacy)

The `scripts/play-sound.sh` script is still available if you want to play sounds independently or use the hardware device:

```bash
./scripts/play-sound.sh waiting
./scripts/play-sound.sh finish
```

You can also call `afplay` directly:

```bash
afplay sound/waiting_user_input.m4a
afplay sound/finish.m4a
```

## Testing

Start the simulator and test the endpoints:

```bash
# Start the server (sound enabled by default on macOS)
cd simulator
node server.js

# In another terminal, test the endpoints
curl http://localhost:3000/waiting   # Should play waiting sound + update LED
curl http://localhost:3000/green     # Should play finish sound + update LED
curl http://localhost:3000/yellow    # No sound (just LED update)
curl http://localhost:3000/red       # No sound (just LED update)
```

You can also test sound files directly:

```bash
# Test individual sound files
afplay sound/waiting_user_input.m4a
afplay sound/finish.m4a

# Or use the helper script
./scripts/play-sound.sh waiting
./scripts/play-sound.sh finish
```

Check the server output to confirm sound is enabled:
```
🔌 Arduino LED Simulator Server
Sound:   ✅ Enabled
```

## Activation

1. Make sure the simulator is running with sound enabled:
   ```bash
   cd simulator
   node server.js
   ```

2. Verify sound is enabled in the startup message:
   ```
   Sound:   ✅ Enabled
   ```

3. Update your `~/.claude/settings.json` with simplified hooks (see example above)

4. **Restart Claude Code** for hooks to take effect

5. Verify hooks are loaded with `/hooks` command

## Server Options

### Disable Sound
If you want to disable sound notifications:

```bash
node server.js --no-sound
```

The server will show:
```
Sound:   ❌ Disabled (--no-sound flag)
```

### Environment Variable (Future)
You can also set an environment variable (if implemented):

```bash
ENABLE_SOUND=false node server.js
```

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
1. **Check server output:** Verify sound is enabled when server starts
   ```
   Sound:   ✅ Enabled
   ```

2. **Check platform:** Sound only works on macOS
   - On other platforms, you'll see: `Sound:   ❌ Disabled (not macOS)`

3. **Check sound files exist:**
   ```bash
   ls -la sound/
   # Should show waiting_user_input.m4a and finish.m4a
   ```

4. **Test afplay directly:**
   ```bash
   which afplay  # Should show /usr/bin/afplay
   afplay sound/waiting_user_input.m4a
   ```

5. **Check system volume:** Make sure your Mac isn't muted

6. **Check server logs:** Look for sound playback messages:
   ```
   🔊 Playing sound: waiting_user_input.m4a
   ```

### Sound plays but LEDs don't work
- Verify simulator is running: `curl http://localhost:3000/status`
- Check web UI at http://localhost:3000 to see if it updates
- If using hardware, make sure you're using the correct IP

### Hooks not triggering
- Verify settings file location: `~/.claude/settings.json` (NOT `~/.claude-code/`)
- Restart Claude Code after editing settings
- Run `/hooks` in Claude Code to see registered hooks
- Check for JSON syntax errors in settings.json

### Server shows "Sound file not found"
Make sure you're running the server from the `simulator/` directory:
```bash
cd simulator
node server.js
```

The server looks for sound files at `../sound/` relative to the simulator directory.

## Hardware Integration

**Important:** The physical ESP8266/ESP32 device **cannot play sounds** because it has no audio hardware. Sound only works with the Node.js simulator running on your Mac.

If you're using the hardware device for LED control, you have two options:

### Option 1: Hardware for LEDs + Simulator for Sound (Recommended)
Keep the simulator running alongside the hardware. Configure hooks to call both:
- Hardware device IP for LED control: `http://192.168.1.100/waiting`
- Localhost for sound: `http://localhost:3000/waiting`

### Option 2: Hardware Only (No Sound)
Replace `localhost:3000` with your device's IP address:
```json
{
  "type": "command",
  "command": "curl -s --max-time 1 http://192.168.1.100/waiting > /dev/null 2>&1",
  "timeout": 2
}
```

Sounds will not play since the hardware cannot play audio.
