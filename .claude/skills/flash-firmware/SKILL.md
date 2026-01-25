---
name: flash-firmware
description: Streamline firmware upload for Clauduino Status ESP8266/ESP32 boards. Use when users need to: (1) Upload firmware/claude_code_led/claude_code_led.ino, (2) Configure Arduino IDE for ESP boards, (3) Set up board manager URLs, (4) Verify serial output at 115200 baud, (5) Troubleshoot upload failures.
---

# Firmware Flashing Guide

Upload Clauduino Status firmware to ESP8266/ESP32 boards.

## Prerequisites

- Arduino IDE 2.x installed
- Hardware assembled (see `/setup-hardware` skill)
- USB cable connected
- Firmware file: `firmware/claude_code_led/claude_code_led.ino`

## Step 1: Detect Board

Ask user which board they're using:
- **ESP8266 NodeMCU** (most common)
- **ESP32 DevKit**

Check connection:
```bash
ls /dev/cu.* | grep -i usb
```

Look for devices like `/dev/cu.usbserial-*` or `/dev/cu.wchusbserial*`

## Step 2: Configure Arduino IDE

### Board Manager URLs

**Arduino IDE → Settings → Additional Board Manager URLs:**

For ESP8266:
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

For ESP32:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

If both, separate with comma or newline.

### Install Board Support

**Tools → Board → Board Manager:**

- Search "ESP8266" → Install "ESP8266 by ESP8266 Community"
- Search "ESP32" → Install "ESP32 by Espressif Systems"

## Step 3: Configure Board Settings

### For ESP8266 NodeMCU:
- **Board:** "NodeMCU 1.0 (ESP-12E Module)"
- **Upload Speed:** 115200
- **CPU Frequency:** 80 MHz
- **Flash Size:** 4MB (FS:2MB OTA:~1019KB)
- **Port:** Select detected USB port

### For ESP32 DevKit:
- **Board:** "ESP32 Dev Module"
- **Upload Speed:** 115200
- **Flash Frequency:** 80MHz
- **Flash Mode:** QIO
- **Flash Size:** 4MB
- **Port:** Select detected USB port

## Step 4: Open and Upload Firmware

1. Open `firmware/claude_code_led/claude_code_led.ino` in Arduino IDE
2. **IMPORTANT:** Update WiFi credentials in code:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Click **Verify** (checkmark) to compile
4. Click **Upload** (arrow) to flash

## Step 5: Monitor Serial Output

Open **Tools → Serial Monitor**:
- Set baud rate: **115200**
- Watch for boot sequence:

**Expected output:**
```
🔴 Clauduino Status LED Server
📶 Connecting to WiFi...
✅ Connected! IP: 192.168.1.XXX
🌐 Server started on port 80
🔴 Status: Idle (Red)
```

**Note the IP address** - needed for integration.

## Verification

Success indicators:
- ✅ Upload completes without errors
- ✅ Serial shows WiFi connection
- ✅ IP address displayed
- ✅ Red LED lights up (idle state)

Test with curl:
```bash
curl http://DEVICE_IP/status
```

Should return JSON with device status.

## Common Upload Issues

**Port not found:**
- Check USB cable (must support data, not power-only)
- Try different USB port
- macOS: Install CH340 or CP210x drivers if needed

**Compilation errors:**
- Ensure correct board selected
- Update board package in Board Manager
- Check Arduino IDE version (2.x recommended)

**Upload timeout:**
- Lower upload speed to 57600
- Press and hold BOOT button during upload (ESP32)
- Check for serial monitor holding port open (close it)

**WiFi not connecting:**
- Verify SSID and password correct
- Check 2.4GHz network (ESP8266/32 don't support 5GHz)
- Look for serial debug messages

## Next Steps

After successful flash:
1. Note the device IP address from serial monitor
2. Test API endpoints (`/test-api` skill)
3. Configure Claude Code integration (`/configure-integration` skill)

## Reference

See `docs/FIRMWARE_GUIDE.md` for detailed instructions and screenshots.
