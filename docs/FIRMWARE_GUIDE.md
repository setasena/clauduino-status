# Firmware Guide

Complete guide to setting up and flashing the Arduino firmware.

## Supported Boards

| Board | Status | Notes |
|-------|--------|-------|
| ESP8266 NodeMCU V3 | Recommended | Most common, well-tested |
| ESP8266 Wemos D1 Mini | Supported | Smaller form factor |
| ESP32 DevKit | Supported | More features, better PWM |

## Arduino IDE Setup

### Install Arduino IDE

Download Arduino IDE 2.x from [arduino.cc](https://www.arduino.cc/en/software)

### Add Board Support

1. Open **File → Preferences**
2. In "Additional Board Manager URLs", add:

   **For ESP8266:**
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```

   **For ESP32:**
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

   (If using both, separate with comma)

3. Go to **Tools → Board → Board Manager**
4. Search and install:
   - "ESP8266" by ESP8266 Community
   - "ESP32" by Espressif Systems (if using ESP32)

### Select Your Board

**ESP8266:**
- Tools → Board → ESP8266 Boards → **NodeMCU 1.0 (ESP-12E Module)**

**ESP32:**
- Tools → Board → ESP32 Arduino → **ESP32 Dev Module**

## Configure the Firmware

Open `firmware/claude_code_led/claude_code_led.ino`

### 1. Select Board Type

Uncomment the correct board definition (line 24-25):

```cpp
// For ESP8266:
#define USE_ESP8266
// #define USE_ESP32

// For ESP32:
// #define USE_ESP8266
#define USE_ESP32
```

### 2. Set WiFi Credentials

Edit lines 28-29:

```cpp
const char* WIFI_SSID = "YourWiFiName";
const char* WIFI_PASSWORD = "YourWiFiPassword";
```

**Important:** WiFi name and password are case-sensitive.

### 3. Adjust Pin Mapping (Optional)

Default pin assignments:

| Board | Red LED | Yellow LED | Green LED |
|-------|---------|------------|-----------|
| ESP8266 | D1 (GPIO5) | D2 (GPIO4) | D3 (GPIO0) |
| ESP32 | GPIO5 | GPIO18 | GPIO19 |

To change pins, edit lines 37-39 (ESP8266) or 47-49 (ESP32).

## Upload Firmware

1. Connect board via USB
2. Select port: **Tools → Port → /dev/cu.usbserial-xxx** (Mac) or **COMx** (Windows)
3. Click **Upload** (right arrow button)
4. Wait for "Done uploading"

### USB Driver Issues

If your board doesn't appear in the port list:

**CH340 chip (most NodeMCU):**
- Download driver from [CH340 driver page](http://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html)

**CP2102 chip (some boards):**
- Download from [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

## Verify Upload

1. Open **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. Press reset button on board

You should see:
```
╔═══════════════════════════════════════════╗
║   Claude Code LED Status Indicator        ║
║   Version 1.0                             ║
╚═══════════════════════════════════════════╝

Connecting to WiFi: YourWiFiName
.....
WiFi Connected!
IP Address: 192.168.1.xxx

Ready! Use these endpoints:
http://192.168.1.xxx/red
http://192.168.1.xxx/yellow
http://192.168.1.xxx/green
```

## PlatformIO Alternative

If you prefer PlatformIO, create `platformio.ini`:

```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200
```

For ESP32:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

## Troubleshooting

**Upload fails with "espcomm_upload_mem failed":**
- Hold FLASH button while uploading
- Try different USB cable
- Check USB driver installation

**WiFi won't connect:**
- Verify credentials (case-sensitive)
- ESP8266 only supports 2.4GHz
- Move closer to router

**LEDs don't light:**
- Check LED polarity (long leg to GPIO)
- Verify resistor connections
- Test GPIO pins with simple blink sketch

See [Troubleshooting Guide](TROUBLESHOOTING.md) for more solutions.
