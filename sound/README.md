# Sound Notifications

This folder contains audio files for Claude Code status notifications on macOS.

## Files

- **waiting_user_input.m4a** - Played when Claude Code is waiting for user input (permission requests, prompts)
- **finish.m4a** - Played when Claude Code completes a task

## Usage

### Quick Test

Play sounds directly with `afplay`:

```bash
afplay waiting_user_input.m4a
afplay finish.m4a
```

### Integration with Claude Code

Use the `scripts/play-sound.sh` helper script in your Claude Code hooks configuration.

See [SOUND_INTEGRATION.md](../docs/SOUND_INTEGRATION.md) for complete setup instructions.

## Customization

You can replace these `.m4a` files with your own sounds. Supported formats:
- .m4a (AAC)
- .mp3
- .wav
- .aiff

Keep the same filenames or update the script paths accordingly.

## Platform Support

Currently supports macOS only (uses `afplay` command). For other platforms:
- **Linux**: Use `aplay`, `paplay`, or `mpg123`
- **Windows**: Use `powershell Start-Process` with Windows Media Player
