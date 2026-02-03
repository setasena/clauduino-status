# Hardware Guide

Shopping list, wiring diagrams, and assembly instructions for the Claude Code LED indicator.

## Essential Components (Minimum Required)

| # | Component | Specs | Qty | Est. Price | Links |
|---|-----------|-------|-----|------------|-------|
| 1 | **ESP8266 NodeMCU V3** | CH340 USB, WiFi 2.4GHz | 1 | $4-8 | [AliExpress](https://aliexpress.com/wholesale?SearchText=nodemcu+v3) / [Amazon](https://amazon.com/s?k=nodemcu+esp8266) |
| 2 | **5mm LED - Red** | 1.8-2.2V, 20mA, diffused | 1 | $0.10 | Included in LED kit |
| 3 | **5mm LED - Yellow** | 2.0-2.2V, 20mA, diffused | 1 | $0.10 | Included in LED kit |
| 4 | **5mm LED - Green** | 2.0-3.2V, 20mA, diffused | 1 | $0.10 | Included in LED kit |
| 5 | **Resistors 220Ω** | 1/4W, through-hole | 3 | $0.15 | Included in resistor kit |
| 6 | **Mini Breadboard** | 170 or 400 tie points | 1 | $1-3 | Usually comes with NodeMCU |
| 7 | **Jumper Wires M-M** | Male-to-male | 6+ | $1-2 | Usually comes with NodeMCU |
| 8 | **Micro USB Cable** | Data capable (not charge-only) | 1 | $1-3 | Check your drawer first! |

**Estimated Total (Essential): $8-20 USD**

---

## Recommended Starter Kits

Instead of buying individual parts, these kits include everything you need:

### Option A: NodeMCU Starter Kit (~$15-25)
Usually includes:
- ✅ NodeMCU ESP8266 board
- ✅ Breadboard
- ✅ Jumper wires
- ✅ LEDs (multiple colors)
- ✅ Resistors (assorted)
- ✅ USB cable

### Option B: ESP32 Starter Kit (~$20-35)
Better choice if you want more features later:
- ✅ ESP32 DevKit
- ✅ All the above
- ✅ More GPIO pins
- ✅ Bluetooth support
- ✅ Better PWM (smoother breathing)

---

## Where to Buy (By Region)

### 🌏 Indonesia / Southeast Asia
| Store | Shipping | Notes |
|-------|----------|-------|
| **Tokopedia** | 1-3 days | Search "NodeMCU ESP8266 kit" |
| **Shopee** | 1-3 days | Often cheaper, check ratings |
| **Bukalapak** | 1-3 days | Good for bulk orders |

**Recommended Search Terms:**
- "NodeMCU ESP8266 starter kit"
- "ESP8266 LED kit lengkap"
- "Arduino LED resistor kit"

### 🌍 International
| Store | Shipping | Price Range |
|-------|----------|-------------|
| **AliExpress** | 2-4 weeks | Cheapest ($5-10) |
| **Amazon** | 1-3 days | Mid-range ($10-20) |
| **Adafruit** | 3-7 days | Premium quality ($15-30) |
| **SparkFun** | 3-7 days | Great documentation ($15-30) |

---

## Optional Upgrades

### Level 2: Better Visual Feedback

| Component | Purpose | Price |
|-----------|---------|-------|
| **WS2812B LED Strip (3-5 LEDs)** | RGB colors, more effects | $3-5 |
| **10mm LEDs** | Bigger, brighter | $1-2 |
| **LED Diffuser Caps** | Softer light | $1 |

### Level 3: Professional Setup

| Component | Purpose | Price |
|-----------|---------|-------|
| **0.96" OLED Display (I2C)** | Show IP, status text | $4-8 |
| **3D Printed Enclosure** | Clean look | $5-15 |
| **Tactile Button** | Manual reset/toggle | $0.50 |
| **Piezo Buzzer** | Audio feedback | $1-2 |

### Level 4: Advanced Features

| Component | Purpose | Price |
|-----------|---------|-------|
| **Neopixel Ring (12 LED)** | Circular progress indicator | $5-10 |
| **ESP32-S3** | Better processor, USB-C | $8-15 |
| **LiPo Battery + Charger** | Portable operation | $5-10 |

---

## Wiring Diagrams

The firmware supports two LED configurations:
- **Option A:** Three separate LEDs (Red, Yellow, Green)
- **Option B:** Single RGB LED (Common Cathode) - recommended for compact builds

---

### Option A: Three Separate LEDs

**Wiring Quick Reference:**
```
ESP8266 Pin    →    Component
───────────────────────────────
D1 (GPIO5)     →    Red LED (+) → 220Ω → GND
D2 (GPIO4)     →    Yellow LED (+) → 220Ω → GND
D3 (GPIO0)     →    Green LED (+) → 220Ω → GND
GND            →    Common ground for all LEDs
```

**Detailed Diagram:**
```
ESP8266          LEDs              Resistors
────────         ────              ─────────

  D1 ──────────── [RED LED] ──────── [220Ω] ──── GND
               (Anode)  (Cathode)

  D2 ──────────── [YELLOW LED] ────── [220Ω] ──── GND
               (Anode)  (Cathode)

  D3 ──────────── [GREEN LED] ─────── [220Ω] ──── GND
               (Anode)  (Cathode)
```

**Firmware config:** Uncomment `#define USE_SEPARATE_LEDS`

---

### Option B: RGB LED (Common Cathode) + Buzzer

This option uses a single 4-leg RGB LED. Yellow is created by mixing Red + Green.

**Components:**
- 1x RGB LED (Common Cathode, 4 legs)
- 1x Active Buzzer (optional, for audio alerts)
- 2x 220Ω resistors

**Wiring Quick Reference:**
```
ESP8266 Pin    →    Component
───────────────────────────────
D1 (GPIO5)     →    RGB Red leg → 220Ω → GND
D2 (GPIO4)     →    RGB Green leg → 220Ω → GND
               →    RGB Blue leg (not connected)
               →    RGB Common leg (longest) → GND
D5 (GPIO14)    →    Buzzer (+)
GND            →    Buzzer (-) and RGB Common
```

**RGB LED Leg Order (typical, looking at flat side):**
```
     ┌───────┐
     │  ●●●  │  (flat edge)
     └─┬┬┬┬──┘
       ││││
       RGCB
       e r o l
       d n m u
         d m e
           o
           n
```
- **R** = Red (to D1 via 220Ω)
- **GND** = Common Cathode, longest leg (to GND)
- **G** = Green (to D2 via 220Ω)
- **B** = Blue (leave unconnected)

**Detailed Diagram:**
```
ESP8266                RGB LED              Resistors
────────               ───────              ─────────

  D1 ─────────────── Red leg ────── [220Ω] ────┐
                                               │
  D2 ─────────────── Green leg ──── [220Ω] ────┤
                                               │
                     Blue leg     (not used)   │
                                               │
  GND ───────────── Common (longest) ──────────┘


  D5 ─────────────── Buzzer (+)
                        │
  GND ───────────── Buzzer (-)
```

**Color Mapping:**
| Status | Red Pin | Green Pin | Result Color |
|--------|---------|-----------|--------------|
| Idle | ON | OFF | Red |
| Processing | PWM | PWM | Yellow (breathing) |
| Waiting | Blink | Blink | Yellow (blinking) |
| Complete | OFF | ON | Green |

**Firmware config:** Uncomment `#define USE_RGB_LED` and `#define USE_BUZZER`

---

### Option C: RGB LED + Buzzer + 16x2 I2C LCD

This is the full-featured setup with visual LED indicator, audio feedback, and text display.

**Additional Components:**
- 1x 16x2 LCD display
- 1x I2C LCD backpack (PCF8574-based)

**Step 1: Solder I2C Backpack to LCD**
1. Align the 16-pin I2C backpack with the LCD's 16-pin header
2. Solder all 16 pins carefully
3. The backpack provides VCC, GND, SDA, SCL connections

**Wiring Quick Reference:**
```
ESP8266 Pin    →    Component
───────────────────────────────
D1 (GPIO5)     →    RGB Red leg → 220Ω → GND
D2 (GPIO4)     →    RGB Green leg → 220Ω → GND
D3 (GPIO0)     →    LCD I2C SDA
D4 (GPIO2)     →    LCD I2C SCL
D5 (GPIO14)    →    Buzzer (+)
3V3            →    LCD I2C VCC
GND            →    LCD I2C GND, Buzzer (-), RGB Common
```

**Detailed Diagram:**
```
ESP8266                Components
────────               ──────────

  3V3 ─────────────── LCD Backpack VCC

  D1 ──────────────── RGB Red ────── [220Ω] ────┐
                                                │
  D2 ──────────────── RGB Green ──── [220Ω] ────┤
                                                │
  D3 ──────────────── LCD SDA                   │
                                                │
  D4 ──────────────── LCD SCL                   │
                                                │
  D5 ──────────────── Buzzer (+)                │
                         │                      │
  GND ─────────────────┬─┴── Buzzer (-)         │
                       │                        │
                       ├──── LCD Backpack GND   │
                       │                        │
                       └──── RGB Common ────────┘
```

**LCD Display Layout:**
```
┌────────────────┐
│Idle         [W]│  ← Row 1: Status + WiFi icon
│192.168.1.100 G│  ← Row 2: IP + Signal quality
└────────────────┘

Signal Quality: G=Good (>-50dBm), F=Fair (-50 to -70dBm), P=Poor (<-70dBm)
```

**I2C Address Troubleshooting:**
- Default address: `0x27`
- Alternative address: `0x3F`
- If LCD doesn't work, run an I2C scanner sketch to detect the correct address
- Edit `LiquidCrystal_I2C lcd(0x27, 16, 2);` in firmware to change address

**Required Library:**
- Install "LiquidCrystal I2C" by Frank de Brabander via Arduino IDE Library Manager

**Firmware config:** Uncomment `#define USE_RGB_LED`, `#define USE_BUZZER`, and `#define USE_LCD`

---

### ESP8266 NodeMCU Pinout Reference

```
┌─────────────────────────────────────┐
│           ESP8266 NodeMCU           │
│                                     │
│  3V3  ●──── LCD VCC ───────●  VIN   │
│  GND  ●──── LCD GND ───────●  GND   │
│  TX   ●────────────────────●  RST   │
│  RX   ●────────────────────●  EN    │
│  D0   ●────────────────────●  3V3   │
│  D1   ●──── LED Red ───────●  GND   │
│  D2   ●──── LED Green ─────●  CLK   │
│  D3   ●──── LCD SDA ───────●  SD0   │
│  D4   ●──── LCD SCL ───────●  CMD   │
│  GND  ●────────────────────●  SD1   │
│  3V3  ●────────────────────●  SD2   │
│  D5   ●──── Buzzer (+) ────●  SD3   │
│  D6   ●────────────────────●  RSV   │
│  D7   ●────────────────────●  RSV   │
│  D8   ●────────────────────●  A0    │
│                                     │
└─────────────────────────────────────┘
```

### ESP32 Pin Mapping

| Function | ESP32 Pin |
|----------|-----------|
| Red LED | GPIO5 |
| Yellow LED | GPIO18 |
| Green LED | GPIO19 |
| Buzzer | GPIO14 |
| LCD SDA | GPIO21 |
| LCD SCL | GPIO22 |

### LED Polarity Guide

```
    Long Leg (+)      Short Leg (-)
        │                  │
        │    ┌──────┐      │
        └────┤ LED  ├──────┘
             └──────┘

Connect: Long leg → GPIO pin
         Short leg → Resistor → GND
```

### Resistor Calculation

Using Ohm's Law: R = (Vcc - Vf) / I

- **Vcc** (ESP8266 GPIO): 3.3V
- **Vf** (LED forward voltage): ~2V
- **I** (desired current): 10-15mA (safe for GPIO)

**Calculation:** R = (3.3V - 2V) / 0.015A = 87Ω (minimum)

**Recommended:** 220Ω or 330Ω for safe operation and good brightness.

| Resistor | Brightness | Safety |
|----------|------------|--------|
| 100Ω | Brightest | Minimum safe |
| 220Ω | Good | Recommended |
| 330Ω | Dimmer | Very safe |
| 470Ω+ | Dim | Overkill |

---

## Pre-Purchase Checklist

Before ordering, verify you have:

- [ ] Computer with USB port (for programming)
- [ ] WiFi network (2.4GHz - ESP8266 doesn't support 5GHz)
- [ ] Arduino IDE installed (free download)
- [ ] Basic soldering iron (optional, for permanent build)

---

## Quick Budget Options

### Ultra Budget (~$5-8)
- 1x NodeMCU from AliExpress
- Use onboard LED only (single status)
- No external components needed

### Budget (~$10-15)
- 1x NodeMCU starter kit from Tokopedia/Shopee
- Everything included

### Standard (~$20-25)
- 1x ESP32 starter kit
- Better board, more expandable

### Premium (~$40-50)
- ESP32 + OLED display
- WS2812B LED strip
- 3D printed case
- Full professional setup

---

## Notes

1. **ESP8266 vs ESP32**: ESP8266 is cheaper and sufficient for this project. ESP32 is better if you plan to add more features later.

2. **Resistor Values**: 220Ω is ideal. 330Ω also works (slightly dimmer). Never use less than 100Ω.

3. **LED Colors**: Diffused LEDs look better than clear/water-clear ones for status indicators.

4. **USB Cable**: Many cheap cables are charge-only and won't work for programming. Test with a known good cable if uploads fail.
