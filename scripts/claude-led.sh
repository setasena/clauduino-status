#!/bin/bash
# claude-led.sh - Claude Code wrapper with LED status indicator
#
# Usage: claude-led [claude arguments...]
# Example: claude-led "explain this code"
#
# Setup:
# 1. Edit DEVICE_IP below to match your LED device
# 2. Make executable: chmod +x claude-led.sh
# 3. Copy to PATH: cp claude-led.sh /usr/local/bin/claude-led
# 4. Run: claude-led "your prompt here"

# ============ CONFIGURATION ============
# Replace with your device IP address (check Serial Monitor after boot)
DEVICE_IP="192.168.1.100"

# Timeout for HTTP requests (seconds) - prevents slow network from blocking Claude
CURL_TIMEOUT=2

# How long to show green LED after completion (seconds)
SUCCESS_DISPLAY_TIME=5
# =======================================

# Function to send status to device (silently, with timeout)
send_status() {
    curl -s --max-time "$CURL_TIMEOUT" "http://$DEVICE_IP/$1" > /dev/null 2>&1
}

# Signal processing started (yellow LED, breathing animation)
send_status "yellow"

# Run Claude Code with all passed arguments
claude "$@"
EXIT_CODE=$?

# Signal completion based on exit code
if [ $EXIT_CODE -eq 0 ]; then
    # Success: show green LED
    send_status "green"

    # Wait, then return to idle (in background so script can exit)
    (sleep "$SUCCESS_DISPLAY_TIME" && send_status "red") &
else
    # Failure: return to idle immediately
    send_status "red"
fi

# Return Claude's exit code
exit $EXIT_CODE
