---
name: setup-hardware
description: "Guide users through complete hardware setup for Clauduino Status IoT LED indicator. Use when users need help with: (1) Wiring ESP8266/ESP32 with LEDs, (2) Pin mapping selection, (3) Resistor calculations, (4) Hardware verification before firmware upload, (5) Physical assembly troubleshooting."
---

# Hardware Setup Guide

Guide users through physical hardware assembly for Clauduino Status.

## Pin Mappings

**ESP8266 (NodeMCU):**
- Red LED: D1 (GPIO5)
- Yellow LED: D2 (GPIO4)
- Green LED: D3 (GPIO0)

**ESP32 (DevKit):**
- Red LED: GPIO5
- Yellow LED: GPIO18
- Green LED: GPIO19

All LEDs connect through 220Ω resistors to ground.

## Setup Workflow

### 1. Identify Board Type

Ask user which board they have:
- ESP8266 NodeMCU (most common)
- ESP32 DevKit

### 2. Gather Components

Required:
- ESP8266/ESP32 board
- 3x LEDs (Red, Yellow, Green)
- 3x 220Ω resistors (red-red-brown)
- Breadboard
- USB cable (micro-USB for ESP8266, USB-C for newer ESP32)
- Jumper wires

### 3. Wiring Instructions

Provide board-specific wiring:

**For each LED:**
1. Insert LED into breadboard (long leg = anode/+, short leg = cathode/-)
2. Connect GPIO pin → LED anode (long leg)
3. Connect LED cathode → 220Ω resistor → Ground rail

**Power:**
- Connect ESP GND → Breadboard ground rail
- USB powers the board (5V from computer)

### 4. Verification Checklist

Before firmware upload, verify:
- [ ] All LEDs have resistors (prevents burnout)
- [ ] LED polarity correct (long leg to GPIO)
- [ ] Ground rail connected to ESP GND
- [ ] No short circuits (LED legs not touching)
- [ ] USB cable provides data (not power-only)

### 5. Visual Inspection

Guide user to check:
- Firm breadboard connections
- Resistor color bands (red-red-brown = 220Ω)
- LED orientation matches diagram
- No loose wires

## Common Issues

**LED polarity confusion:**
- Long leg = anode (+) = connects to GPIO
- Short leg = cathode (-) = connects to resistor/ground
- If LED has flat side on plastic case, flat side = cathode

**Wrong resistor value:**
- 220Ω is standard (red-red-brown-gold)
- 330Ω also works (orange-orange-brown)
- Don't use <100Ω (too bright, short LED life)

**Power issues:**
- Some USB cables are power-only, need data cable
- ESP32 may need external 5V if USB insufficient

## Reference Documentation

For detailed diagrams and photos, read: `docs/HARDWARE_GUIDE.md`

For wiring diagrams, use `references/wiring-diagrams.md` if available.

## Next Steps

After hardware setup verified:
1. Keep USB disconnected
2. Move to firmware flashing (`/flash-firmware` skill)
3. Test with API once firmware running
