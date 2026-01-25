#!/bin/bash
# Helper script to play sound notifications for Claude Code status changes

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
SOUND_DIR="$PROJECT_ROOT/sound"

# Play sound based on argument
case "$1" in
  waiting|prompt)
    afplay "$SOUND_DIR/waiting_user_input.m4a" &
    ;;
  finish|complete)
    afplay "$SOUND_DIR/finish.m4a" &
    ;;
  *)
    echo "Usage: $0 {waiting|prompt|finish|complete}"
    exit 1
    ;;
esac
