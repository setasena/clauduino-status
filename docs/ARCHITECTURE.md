# Architecture Reference

> **Note:** This is the original project specification, preserved as a technical reference.
> For practical guides, see:
> - [Quick Start](QUICK_START.md) - Get running fast
> - [Hardware Guide](HARDWARE_GUIDE.md) - Shopping and wiring
> - [Firmware Guide](FIRMWARE_GUIDE.md) - Arduino setup
> - [Integration Guide](INTEGRATION_GUIDE.md) - Connect to Claude Code
> - [API Reference](API_REFERENCE.md) - All endpoints
> - [Troubleshooting](TROUBLESHOOTING.md) - Common issues

---

# Claude Code LED Status Indicator - IoT Project Specification

## Project Overview

Build an IoT device that visually indicates the operational state of Claude Code through a physical LED display. The device connects to a local network and receives HTTP commands to change LED states, providing real-time visual feedback for developers using Claude Code.

## Objectives

The primary goal is to create a physical status indicator that shows three distinct states:

1. **Idle State (Red LED)** - Claude Code is waiting for user input
2. **Processing State (Yellow LED)** - Claude Code is actively working/thinking, displayed with a breathing/pulsing animation
3. **Complete State (Green LED)** - Claude Code has finished the task successfully

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Developer Workstation                         │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────┐    HTTP Request    ┌──────────────────────────┐  │
│  │              │ ──────────────────▶ │                          │  │
│  │  Claude Code │                     │   ESP8266/ESP32 Board    │  │
│  │   (CLI)      │    /yellow          │                          │  │
│  │              │    /green           │   ┌───┐ ┌───┐ ┌───┐     │  │
│  └──────────────┘    /red             │   │ R │ │ Y │ │ G │     │  │
│         │                             │   │ E │ │ E │ │ R │     │  │
│         │                             │   │ D │ │ L │ │ E │     │  │
│         ▼                             │   └───┘ └───┘ └───┘     │  │
│  ┌──────────────┐                     │      LED Array           │  │
│  │   Wrapper    │                     └──────────────────────────┘  │
│  │   Script     │                                │                   │
│  │  (bash/hook) │                                │ WiFi              │
│  └──────────────┘                                │                   │
│                                                  ▼                   │
│                                    ┌─────────────────────────┐      │
│                                    │    Local Network        │      │
│                                    │    (Same WiFi/LAN)      │      │
│                                    └─────────────────────────┘      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## Functional Requirements

### FR-001: LED State Control

The device must support three distinct LED states controllable via HTTP endpoints:

| Endpoint | LED State | Visual Effect | Description |
|----------|-----------|---------------|-------------|
| `GET /red` | Red ON, others OFF | Solid | Idle/waiting state |
| `GET /yellow` | Yellow ON, others OFF | Breathing/pulsing animation | Processing state |
| `GET /green` | Green ON, others OFF | Solid | Task complete state |
| `GET /status` | N/A | N/A | Returns current state as JSON |

### FR-002: Breathing Animation

The yellow LED must implement a breathing effect during processing state:

- Fade in duration: 750ms (0% to 100% brightness)
- Fade out duration: 750ms (100% to 0% brightness)
- Total cycle: 1500ms
- Use PWM (Pulse Width Modulation) for smooth transitions

### FR-003: Network Connectivity

- Connect to local WiFi network (2.4GHz)
- Obtain IP address via DHCP
- Print IP address to Serial monitor on boot
- Respond to HTTP requests within 100ms

### FR-004: State Persistence

- Device should maintain current LED state until changed
- Default to Red (Idle) state on boot
- No state persistence across power cycles required

## Non-Functional Requirements

### NFR-001: Response Time

HTTP endpoint response time must be less than 100ms under normal conditions.

### NFR-002: Power Consumption

Device should operate safely on USB power (5V, 500mA max).

### NFR-003: Reliability

Device should auto-reconnect to WiFi if connection is lost.

## Hardware Components

### Required Components

| Component | Quantity | Specifications | Estimated Cost (USD) |
|-----------|----------|----------------|---------------------|
| ESP8266 NodeMCU or ESP32 DevKit | 1 | WiFi-enabled microcontroller, 3.3V logic | $5-10 |
| Red LED (5mm) | 1 | Forward voltage: 1.8-2.2V, 20mA | $0.10 |
| Yellow LED (5mm) | 1 | Forward voltage: 2.0-2.2V, 20mA | $0.10 |
| Green LED (5mm) | 1 | Forward voltage: 2.0-3.2V, 20mA | $0.10 |
| Resistors (220Ω or 330Ω) | 3 | 1/4W, through-hole | $0.05 |
| Breadboard | 1 | Mini or half-size | $2-5 |
| Jumper Wires | ~10 | Male-to-male | $1-2 |
| Micro USB Cable | 1 | Data + power capable | $1-3 |

**Total Estimated Cost: $10-25 USD**

### Optional Enhancements

| Component | Purpose | Estimated Cost (USD) |
|-----------|---------|---------------------|
| 3D Printed Enclosure | Professional housing | $5-15 |
| WS2812B RGB LED Strip (3 LEDs) | Addressable RGB, more effects | $3-5 |
| OLED Display (0.96" I2C) | Show status text/IP address | $5-8 |
| Push Button | Manual state toggle/reset | $0.20 |
| Buzzer | Audio feedback on state change | $1-2 |

## Pin Configuration

### ESP8266 NodeMCU Pinout

```
┌─────────────────────────────────────┐
│           ESP8266 NodeMCU           │
│                                     │
│  3V3  ●────────────────────●  VIN   │
│  GND  ●────────────────────●  GND   │
│  TX   ●────────────────────●  RST   │
│  RX   ●────────────────────●  EN    │
│  D0   ●────────────────────●  3V3   │
│  D1   ●──── RED LED ───────●  GND   │
│  D2   ●──── YELLOW LED ────●  CLK   │
│  D3   ●──── GREEN LED ─────●  SD0   │
│  D4   ●────────────────────●  CMD   │
│  GND  ●────────────────────●  SD1   │
│  3V3  ●────────────────────●  SD2   │
│  D5   ●────────────────────●  SD3   │
│  D6   ●────────────────────●  RSV   │
│  D7   ●────────────────────●  RSV   │
│  D8   ●────────────────────●  A0    │
│                                     │
└─────────────────────────────────────┘
```

### Wiring Diagram

```
ESP8266          LEDs              Resistors
────────         ────              ─────────

  D1 ──────────── [RED LED] ──────── [220Ω] ──── GND
               (Anode)  (Cathode)

  D2 ──────────── [YELLOW LED] ────── [220Ω] ──── GND
               (Anode)  (Cathode)

  D3 ──────────── [GREEN LED] ─────── [220Ω] ──── GND
               (Anode)  (Cathode)

Note: LED longer leg = Anode (positive), connect to GPIO pin
      LED shorter leg = Cathode (negative), connect through resistor to GND
```

### Resistor Calculation

Using Ohm's Law: R = (Vcc - Vf) / I

- Vcc (ESP8266 GPIO): 3.3V
- Vf (LED forward voltage): ~2V
- I (desired current): 10-15mA (safe for GPIO)

R = (3.3V - 2V) / 0.015A = 87Ω (minimum)

**Recommended: 220Ω or 330Ω** for safe operation and good brightness.

## Software Architecture

### Arduino Firmware Structure

```
claude-code-led/
├── claude-code-led.ino      # Main Arduino sketch
├── config.h                  # WiFi credentials and pin definitions
├── led_controller.h          # LED control functions
├── led_controller.cpp        # LED control implementation
├── web_server.h              # HTTP server setup
└── web_server.cpp            # HTTP endpoint handlers
```

### Core Functions

```cpp
// LED Control
void setLED(String status);           // Set LED state: "idle", "processing", "complete"
void breathingEffect();               // PWM breathing animation for yellow LED
void allOff();                        // Turn all LEDs off

// Network
void setupWiFi();                     // Connect to WiFi network
void setupWebServer();                // Initialize HTTP endpoints
void handleClient();                  // Process incoming HTTP requests

// Endpoints
void handleRed();                     // GET /red - Set idle state
void handleYellow();                  // GET /yellow - Set processing state
void handleGreen();                   // GET /green - Set complete state
void handleStatus();                  // GET /status - Return current state
```

### State Machine

```
                    ┌─────────────────────────────────────────┐
                    │                                         │
                    ▼                                         │
    ┌─────────┐  /yellow  ┌──────────────┐  /green  ┌─────────────┐
    │  IDLE   │ ────────▶ │  PROCESSING  │ ───────▶ │  COMPLETE   │
    │  (Red)  │           │  (Yellow)    │          │  (Green)    │
    └─────────┘           └──────────────┘          └─────────────┘
         ▲                       │                        │
         │                       │                        │
         │        /red           │         /red           │
         └───────────────────────┴────────────────────────┘
```

## API Specification

### Base URL

```
http://<DEVICE_IP>:80
```

### Endpoints

#### Set Idle State

```http
GET /red
GET /idle
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/plain

OK - Status: IDLE (Red LED)
```

#### Set Processing State

```http
GET /yellow
GET /processing
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/plain

OK - Status: PROCESSING (Yellow LED)
```

#### Set Complete State

```http
GET /green
GET /complete
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: text/plain

OK - Status: COMPLETE (Green LED)
```

#### Get Current Status

```http
GET /status
```

**Response:**
```json
{
  "status": "processing",
  "uptime": 3600,
  "ip": "192.168.1.100"
}
```

## Claude Code Integration

### Method 1: Wrapper Script (Recommended)

Create a bash wrapper that sends HTTP requests before and after Claude Code execution:

```bash
#!/bin/bash
# claude-led.sh

DEVICE_IP="192.168.1.100"  # Replace with your device IP

# Signal processing started
curl -s "http://$DEVICE_IP/yellow" > /dev/null

# Run Claude Code
claude "$@"
EXIT_CODE=$?

# Signal completion
if [ $EXIT_CODE -eq 0 ]; then
    curl -s "http://$DEVICE_IP/green" > /dev/null
    sleep 5
fi

# Return to idle
curl -s "http://$DEVICE_IP/red" > /dev/null

exit $EXIT_CODE
```

### Method 2: Shell Alias

Add to `~/.bashrc` or `~/.zshrc`:

```bash
alias claude='_claude_with_led'

_claude_with_led() {
    local DEVICE_IP="192.168.1.100"
    curl -s "http://$DEVICE_IP/yellow" > /dev/null 2>&1
    command claude "$@"
    local exit_code=$?
    if [ $exit_code -eq 0 ]; then
        curl -s "http://$DEVICE_IP/green" > /dev/null 2>&1
        (sleep 5 && curl -s "http://$DEVICE_IP/red" > /dev/null 2>&1) &
    else
        curl -s "http://$DEVICE_IP/red" > /dev/null 2>&1
    fi
    return $exit_code
}
```

### Method 3: Claude Code Hooks (If Available)

Configure in Claude Code settings file:

```json
{
  "hooks": {
    "preExecution": "curl -s http://DEVICE_IP/yellow",
    "postExecution": "curl -s http://DEVICE_IP/green && sleep 3 && curl -s http://DEVICE_IP/red"
  }
}
```

## Testing Plan

### Unit Tests

| Test ID | Description | Expected Result |
|---------|-------------|-----------------|
| T-001 | Send GET /red | Red LED ON, others OFF, HTTP 200 |
| T-002 | Send GET /yellow | Yellow LED breathing, others OFF, HTTP 200 |
| T-003 | Send GET /green | Green LED ON, others OFF, HTTP 200 |
| T-004 | Send GET /status | JSON response with current state |
| T-005 | Invalid endpoint | HTTP 404 response |
| T-006 | Device reboot | Starts in idle (red) state |

### Integration Tests

| Test ID | Description | Expected Result |
|---------|-------------|-----------------|
| T-101 | Run wrapper script with successful command | Yellow → Green → Red sequence |
| T-102 | Run wrapper script with failing command | Yellow → Red sequence |
| T-103 | Rapid state changes | Device handles without crashing |
| T-104 | WiFi disconnect/reconnect | Device auto-reconnects |

### Manual Testing Commands

```bash
# Test individual endpoints
curl http://DEVICE_IP/red
curl http://DEVICE_IP/yellow
curl http://DEVICE_IP/green
curl http://DEVICE_IP/status

# Test full sequence
curl http://DEVICE_IP/yellow && sleep 3 && curl http://DEVICE_IP/green && sleep 2 && curl http://DEVICE_IP/red
```

## Development Setup

### Prerequisites

1. Arduino IDE 2.x or PlatformIO
2. ESP8266 or ESP32 board support package
3. USB drivers for your board (CH340 or CP2102)

### Arduino IDE Setup

1. Open Arduino IDE
2. Go to File → Preferences
3. Add to "Additional Board Manager URLs":
   - ESP8266: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - ESP32: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
4. Go to Tools → Board → Board Manager
5. Search and install "ESP8266" or "ESP32"
6. Select your board from Tools → Board menu

### PlatformIO Setup (Alternative)

```ini
; platformio.ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200
```

## Deployment Checklist

- [ ] Hardware assembled and wired correctly
- [ ] LEDs tested individually (polarity check)
- [ ] WiFi credentials configured in code
- [ ] Firmware uploaded successfully
- [ ] Device IP address noted from Serial monitor
- [ ] HTTP endpoints tested via curl
- [ ] Wrapper script configured with correct IP
- [ ] Breathing animation verified on yellow LED
- [ ] End-to-end test with Claude Code completed

## Troubleshooting Guide

### LED Not Lighting Up

1. Check LED polarity (longer leg to GPIO, shorter to resistor)
2. Verify resistor connections
3. Test with simple blink sketch first
4. Check GPIO pin assignments in code

### Cannot Connect to WiFi

1. Verify WiFi credentials (case-sensitive)
2. Ensure 2.4GHz network (ESP8266 doesn't support 5GHz)
3. Check WiFi signal strength
4. Try moving device closer to router

### HTTP Requests Not Working

1. Verify device IP address (check Serial monitor)
2. Ensure device and computer on same network
3. Check firewall settings
4. Test with `ping DEVICE_IP` first

### Breathing Effect Not Smooth

1. Ensure using PWM-capable pin
2. Reduce delay values in breathing loop
3. Check for other blocking code in loop()

## Future Enhancements

### Phase 2 Features

1. **mDNS Support** - Access via `http://claude-led.local` instead of IP
2. **OTA Updates** - Update firmware over WiFi
3. **Configuration Portal** - Web-based WiFi setup
4. **Multiple Animation Modes** - Chase, blink, rainbow for RGB

### Phase 3 Features

1. **MQTT Integration** - For more complex IoT setups
2. **Home Assistant Integration** - Smart home compatibility
3. **Mobile App** - iOS/Android status viewer
4. **Multi-device Sync** - Multiple indicators in sync

## References

- [ESP8266 Arduino Core Documentation](https://arduino-esp8266.readthedocs.io/)
- [ESP32 Arduino Core Documentation](https://docs.espressif.com/projects/arduino-esp32/)
- [Claude Code Documentation](https://docs.anthropic.com/claude-code)
- [PWM on ESP8266](https://randomnerdtutorials.com/esp8266-pwm-arduino-ide/)

---

**Document Version:** 1.0  
**Last Updated:** January 2025  
**Author:** Generated for IoT Development
