# Integration Guide

Connect your LED indicator to Claude Code.

## Prerequisites

- LED device is powered on and connected to WiFi
- You have the device IP address (from Serial Monitor)
- Device responds to `curl http://DEVICE_IP/status`

## Method 1: Wrapper Script (Recommended)

Use the provided wrapper script for the most reliable integration.

### Setup

1. Copy the script to your PATH:
   ```bash
   cp scripts/claude-led.sh /usr/local/bin/claude-led
   chmod +x /usr/local/bin/claude-led
   ```

2. Edit the script and set your device IP:
   ```bash
   DEVICE_IP="192.168.1.xxx"  # Your device IP
   ```

3. Use `claude-led` instead of `claude`:
   ```bash
   claude-led "explain this code"
   ```

### Script Behavior

1. Sends `/yellow` when Claude starts processing
2. Runs your Claude command
3. Sends `/green` on success (exit code 0)
4. Waits 5 seconds, then sends `/red` to return to idle
5. On failure, immediately returns to `/red`

## Method 2: Shell Alias

Add Claude Code integration directly to your shell.

### For Zsh (~/.zshrc)

```bash
export CLAUDE_LED_IP="192.168.1.xxx"

alias claude='_claude_with_led'

_claude_with_led() {
    curl -s "http://$CLAUDE_LED_IP/yellow" > /dev/null 2>&1
    command claude "$@"
    local exit_code=$?
    if [ $exit_code -eq 0 ]; then
        curl -s "http://$CLAUDE_LED_IP/green" > /dev/null 2>&1
        (sleep 5 && curl -s "http://$CLAUDE_LED_IP/red" > /dev/null 2>&1) &
    else
        curl -s "http://$CLAUDE_LED_IP/red" > /dev/null 2>&1
    fi
    return $exit_code
}
```

### For Bash (~/.bashrc)

Same as above, but use `~/.bashrc` instead.

### Reload Shell

```bash
source ~/.zshrc   # or ~/.bashrc
```

## Method 3: Claude Code Hooks

If Claude Code supports hooks in your configuration, you can use them for automatic integration.

### Configuration (~/.claude/settings.json)

```json
{
  "hooks": {
    "preExecution": "curl -s http://DEVICE_IP/yellow > /dev/null",
    "postExecution": "curl -s http://DEVICE_IP/green > /dev/null && sleep 3 && curl -s http://DEVICE_IP/red > /dev/null"
  }
}
```

**Note:** Replace `DEVICE_IP` with your actual device IP address.

## Testing Your Integration

### Test Individual States

```bash
# Should turn yellow (breathing)
curl http://192.168.1.xxx/yellow

# Should turn green
curl http://192.168.1.xxx/green

# Should turn red
curl http://192.168.1.xxx/red

# Check current status
curl http://192.168.1.xxx/status
```

### Test Full Flow

```bash
# Run a simple Claude command
claude "what is 2+2"

# Watch the LEDs:
# 1. Yellow breathing while processing
# 2. Green when complete
# 3. Red after 5 seconds
```

## Troubleshooting Integration

### LED doesn't change when running Claude

1. Test direct curl command:
   ```bash
   curl http://192.168.1.xxx/yellow
   ```
   If this works, the issue is in your script/alias.

2. Check the IP address is correct:
   ```bash
   ping 192.168.1.xxx
   ```

3. Verify the alias is loaded:
   ```bash
   type claude
   # Should show the function, not just "claude"
   ```

### LED stays yellow

- Check if Claude command is still running
- The script might be waiting for Claude to finish
- Try `Ctrl+C` to interrupt and check if it returns to red

### Network timeout slowing down Claude

Add shorter timeout to curl:
```bash
curl -s --max-time 1 "http://$CLAUDE_LED_IP/yellow" > /dev/null 2>&1
```

This ensures slow/failed network requests don't delay Claude.

## Advanced: Multiple Devices

If you have multiple LED indicators:

```bash
export CLAUDE_LED_IP_OFFICE="192.168.1.100"
export CLAUDE_LED_IP_HOME="192.168.1.101"

# Use the appropriate one
alias claude='_claude_with_led $CLAUDE_LED_IP_OFFICE'
```

## Next Steps

- [API Reference](API_REFERENCE.md) - All available endpoints
- [Troubleshooting](TROUBLESHOOTING.md) - Common issues and fixes
