# Integration Guide

Connect your LED indicator to Claude Code.

## Prerequisites

- LED device is powered on and connected to WiFi
- You have the device IP address (from Serial Monitor) or using simulator at `localhost:3000`
- Device responds to `curl http://DEVICE_IP/status`

## Method 1: Claude Code Hooks (Recommended for Interactive Sessions)

This method automatically integrates with Claude Code's interactive sessions using hooks.

### Setup

1. Create or edit `~/.claude/settings.json`:
   ```bash
   mkdir -p ~/.claude
   nano ~/.claude/settings.json
   ```

2. Add the following configuration:

   **For Hardware Device:**
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

3. **Important:** Replace `192.168.1.xxx` with your actual device IP address.

4. Save the file and **restart Claude Code** (hooks are loaded at startup).

5. Verify hooks are registered:
   ```bash
   claude
   ```
   Then type `/hooks` in the Claude Code session to see registered hooks.

### How It Works

- `UserPromptSubmit` - Triggers when you send a message → Yellow LED (processing)
- `Stop` - Triggers when Claude finishes responding → Green LED (complete) → Red LED after 5s (idle)

### Troubleshooting Hooks

**Hooks not firing:**
1. Verify file location: `cat ~/.claude/settings.json`
2. Check you restarted Claude Code after creating/editing settings
3. Run `/hooks` command in Claude Code to verify registration
4. Test the curl command manually: `curl http://localhost:3000/yellow`

**Debug mode:**
```bash
claude --debug
```
This shows detailed hook execution logs.

## Method 2: Wrapper Script (For Non-Interactive Commands)

Use the provided wrapper script when running Claude Code non-interactively.

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

## Method 3: Shell Alias

Add Claude Code integration directly to your shell (alternative to hooks).

### For Zsh (~/.zshrc)

```bash
export CLAUDE_LED_IP="192.168.1.xxx"

alias claude='_claude_with_led'

_claude_with_led() {
    curl -s --max-time 1 "http://$CLAUDE_LED_IP/yellow" > /dev/null 2>&1
    command claude "$@"
    local exit_code=$?
    if [ $exit_code -eq 0 ]; then
        curl -s --max-time 1 "http://$CLAUDE_LED_IP/green" > /dev/null 2>&1
        (sleep 5 && curl -s --max-time 1 "http://$CLAUDE_LED_IP/red" > /dev/null 2>&1) &
    else
        curl -s --max-time 1 "http://$CLAUDE_LED_IP/red" > /dev/null 2>&1
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

**Note:** This method only works for non-interactive Claude commands, not interactive sessions.

## Testing Your Integration

### Test Individual States

Test the API endpoints directly:

```bash
# Should turn yellow (breathing)
curl http://192.168.1.xxx/yellow
# Or for simulator:
curl http://localhost:3000/yellow

# Should turn green
curl http://192.168.1.xxx/green

# Should turn red
curl http://192.168.1.xxx/red

# Check current status (JSON response)
curl http://192.168.1.xxx/status
```

### Test Hooks (Method 1)

After configuring `~/.claude/settings.json` and restarting Claude Code:

1. Start an interactive Claude Code session:
   ```bash
   claude
   ```

2. Verify hooks are loaded:
   ```
   /hooks
   ```
   You should see `UserPromptSubmit` and `Stop` hooks listed.

3. Send a test message:
   ```
   hello claude
   ```

4. **Expected behavior:**
   - 🟡 Yellow LED turns on immediately (breathing animation)
   - 🟢 Green LED when Claude finishes responding
   - 🔴 Red LED after 5 seconds

5. **Check simulator server logs** - you should see:
   ```
   🟡 Status → PROCESSING (from GET /yellow)
   🟢 Status → COMPLETE (from GET /green)
   🔴 Status → IDLE (from GET /red)
   ```

### Test Wrapper Script (Method 2)

```bash
# Run a simple Claude command
claude-led "what is 2+2"

# Watch the LEDs:
# 1. Yellow breathing while processing
# 2. Green when complete
# 3. Red after 5 seconds
```

## Troubleshooting Integration

### Hooks not firing (Method 1)

1. **Verify settings file location:**
   ```bash
   cat ~/.claude/settings.json
   ```
   Must be `~/.claude/settings.json` NOT `~/.claude-code/settings.json`

2. **Check hooks are registered:**
   Start Claude Code and run `/hooks` command. You should see your hooks listed.

3. **Test the curl command manually:**
   ```bash
   curl http://localhost:3000/yellow
   ```
   If this works, the issue is with hook registration.

4. **Verify you restarted Claude Code:**
   Hooks are only loaded at startup. Exit and restart your Claude Code session.

5. **Enable debug mode:**
   ```bash
   claude --debug
   ```
   This shows detailed hook execution logs.

6. **Check device/simulator is running:**
   ```bash
   curl http://localhost:3000/status
   # Or for hardware:
   curl http://192.168.1.xxx/status
   ```

### LED doesn't change with wrapper script (Method 2)

1. Test direct curl command:
   ```bash
   curl http://192.168.1.xxx/yellow
   ```
   If this works, the issue is in your script.

2. Check the IP address is correct:
   ```bash
   ping 192.168.1.xxx
   ```

3. Verify the script is executable:
   ```bash
   ls -la /usr/local/bin/claude-led
   which claude-led
   ```

### Shell alias not working (Method 3)

1. Verify the alias is loaded:
   ```bash
   type claude
   # Should show the function definition, not just "claude is /usr/local/bin/claude"
   ```

2. Reload your shell config:
   ```bash
   source ~/.zshrc  # or ~/.bashrc
   ```

### LED stays yellow

- Check if Claude command is still running
- The script/hook might be waiting for Claude to finish
- Try `Ctrl+C` to interrupt and check if it returns to red
- For hooks: the `Stop` hook fires when Claude finishes, not when you interrupt

### Network timeout slowing down Claude

The configuration already includes `--max-time 1` timeout to prevent slow network requests from blocking Claude:

```bash
curl -s --max-time 1 "http://DEVICE_IP/yellow" > /dev/null 2>&1
```

This ensures failed/slow requests don't delay Claude Code responses.

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
