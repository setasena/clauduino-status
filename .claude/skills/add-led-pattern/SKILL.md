---
name: add-led-pattern
description: "Add new LED animation patterns to Clauduino Status firmware. Use when users want to: (1) Create custom LED animations, (2) Add new API endpoints for LED states, (3) Implement new visual patterns (fade, pulse, strobe, etc.), (4) Extend firmware with additional states, (5) Update documentation for new patterns."
---

# Add LED Pattern

Add new LED animation patterns to Clauduino Status firmware.

## Prerequisites

- Understanding of desired animation (breathing, blinking, fade, pulse, strobe, rainbow, etc.)
- Familiarity with Arduino C++ basics
- Firmware file: `firmware/claude_code_led/claude_code_led.ino`

## Pattern Implementation Workflow

### 1. Define Pattern Requirements

Gather specifications:
- **Animation name** (e.g., "pulse", "fade", "strobe")
- **Which LED(s)** (red, yellow, green, or multiple)
- **Timing** (speed, interval, duration)
- **Visual behavior** (smooth, stepped, on/off, etc.)
- **Endpoint name** (e.g., `/pulse`, `/strobe`)

### 2. Add Global Variables

In the `GLOBAL VARIABLES` section, add pattern-specific variables:

```cpp
// [New Pattern] effect variables
bool isNewPattern = false;
int patternVariable = 0;
unsigned long lastPatternUpdate = 0;
const int PATTERN_INTERVAL = 50;  // ms between updates
```

**Example for pulse pattern:**
```cpp
// Pulse effect variables
bool isPulsing = false;
int pulseCounter = 0;
unsigned long lastPulseUpdate = 0;
const int PULSE_INTERVAL = 100;  // Faster than breathing
```

### 3. Create Update Function

Add animation update function after `updateBlinking()`:

```cpp
void updateNewPattern() {
  if (!isNewPattern) return;

  unsigned long currentTime = millis();
  if (currentTime - lastPatternUpdate < PATTERN_INTERVAL) return;

  lastPatternUpdate = currentTime;

  // Animation logic here
  // Update LED brightness or state

  #ifdef USE_ESP8266
    analogWrite(TARGET_PIN, brightness);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, brightness);
  #endif
}
```

**Example pulse pattern:**
```cpp
void updatePulsing() {
  if (!isPulsing) return;

  unsigned long currentTime = millis();
  if (currentTime - lastPulseUpdate < PULSE_INTERVAL) return;

  lastPulseUpdate = currentTime;

  // Quick pulse: 0 -> 255 -> 0
  pulseCounter++;
  int brightness = (pulseCounter % 20 < 10) ? (pulseCounter % 10) * 25 : (10 - pulseCounter % 10) * 25;

  #ifdef USE_ESP8266
    analogWrite(YELLOW_PIN, brightness);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, brightness);
  #endif
}
```

### 4. Update allLedsOff()

Add flag reset to `allLedsOff()`:

```cpp
void allLedsOff() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  #ifdef USE_ESP8266
    analogWrite(YELLOW_PIN, 0);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, 0);
  #endif

  isBreathing = false;
  isBlinking = false;
  isNewPattern = false;  // Add this line
}
```

### 5. Add State Handler

Add new case to `setLedState()` function:

```cpp
void setLedState(String status) {
  allLedsOff();
  currentStatus = status;

  // ... existing cases ...

  else if (status == "newpattern" || status == "alias") {
    currentStatus = "newpattern";
    isNewPattern = true;
    patternVariable = 0;  // Initialize
    lastPatternUpdate = millis();
    Serial.println("📍 Status: NEW_PATTERN (Description)");
  }
}
```

**Example for pulse:**
```cpp
else if (status == "pulse") {
  currentStatus = "pulse";
  isPulsing = true;
  pulseCounter = 0;
  lastPulseUpdate = millis();
  Serial.println("📍 Status: PULSE (Yellow LED - Fast Pulse)");
}
```

### 6. Create HTTP Handler

Add HTTP handler function:

```cpp
void handleNewPattern() {
  setLedState("newpattern");
  server.send(200, "text/plain", "OK - Status: NEW_PATTERN");
}
```

### 7. Register Route

In `setupWebServer()`, add route:

```cpp
void setupWebServer() {
  // Existing routes...
  server.on("/newpattern", handleNewPattern);
  server.on("/alias", handleNewPattern);  // Optional alias

  // ... rest of setup
}
```

### 8. Update Main Loop

Add update function call in `loop()`:

```cpp
void loop() {
  server.handleClient();

  updateBreathing();
  updateBlinking();
  updateNewPattern();  // Add this line

  // ... rest of loop
}
```

### 9. Test Pattern

Upload firmware and test:

```bash
curl http://DEVICE_IP/newpattern
```

Verify:
- Animation runs smoothly
- Serial monitor shows correct status
- No interference with other patterns
- Works on both ESP8266 and ESP32

### 10. Update Documentation

Update files:

**CLAUDE.md** - Add endpoint to API table:
```markdown
| `/newpattern` | `/alias` | Description of LED behavior |
```

**docs/API_REFERENCE.md** - Document endpoint:
```markdown
### GET /newpattern

Description of what it does.

**Aliases:** `/alias`

**Response:** `200 OK` with status message
```

**README.md** - Add to features list if significant

## Pattern Templates

### Fade In/Out Pattern

```cpp
// Fade variables
bool isFading = false;
int fadeBrightness = 0;
int fadeTarget = 255;
unsigned long lastFadeUpdate = 0;
const int FADE_INTERVAL = 10;

void updateFading() {
  if (!isFading) return;

  unsigned long currentTime = millis();
  if (currentTime - lastFadeUpdate < FADE_INTERVAL) return;

  lastFadeUpdate = currentTime;

  if (fadeBrightness < fadeTarget) {
    fadeBrightness += 5;
  } else if (fadeBrightness > fadeTarget) {
    fadeBrightness -= 5;
  }

  #ifdef USE_ESP8266
    analogWrite(YELLOW_PIN, fadeBrightness);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, fadeBrightness);
  #endif
}
```

### Strobe Pattern

```cpp
// Strobe variables
bool isStrobing = false;
bool strobeState = false;
unsigned long lastStrobeTime = 0;
const int STROBE_INTERVAL = 100;  // Fast strobe

void updateStrobing() {
  if (!isStrobing) return;

  unsigned long currentTime = millis();
  if (currentTime - lastStrobeTime < STROBE_INTERVAL) return;

  lastStrobeTime = currentTime;
  strobeState = !strobeState;

  int brightness = strobeState ? 255 : 0;

  #ifdef USE_ESP8266
    analogWrite(YELLOW_PIN, brightness);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, brightness);
  #endif
}
```

### Multi-LED Pattern

```cpp
// Rainbow cycle variables
bool isRainbow = false;
int rainbowPhase = 0;
unsigned long lastRainbowUpdate = 0;
const int RAINBOW_INTERVAL = 50;

void updateRainbow() {
  if (!isRainbow) return;

  unsigned long currentTime = millis();
  if (currentTime - lastRainbowUpdate < RAINBOW_INTERVAL) return;

  lastRainbowUpdate = currentTime;
  rainbowPhase = (rainbowPhase + 1) % 9;  // 9 phases

  // Cycle through R->Y->G pattern
  allLedsOff();
  switch (rainbowPhase / 3) {
    case 0: digitalWrite(RED_PIN, HIGH); break;
    case 1:
      #ifdef USE_ESP8266
        analogWrite(YELLOW_PIN, 255);
      #endif
      #ifdef USE_ESP32
        ledcWrite(PWM_CHANNEL, 255);
      #endif
      break;
    case 2: digitalWrite(GREEN_PIN, HIGH); break;
  }
}
```

## ESP8266 vs ESP32 Compatibility

**Key difference:** PWM control

**ESP8266:** Use `analogWrite(pin, value)`
**ESP32:** Use `ledcWrite(channel, value)`

Always use `#ifdef` guards:

```cpp
#ifdef USE_ESP8266
  analogWrite(YELLOW_PIN, brightness);
#endif
#ifdef USE_ESP32
  ledcWrite(PWM_CHANNEL, brightness);
#endif
```

## Common Issues

**Animation too fast/slow:**
- Adjust `INTERVAL` constant (larger = slower)
- Typical range: 10ms (very fast) to 500ms (slow)

**Brightness steps visible:**
- Reduce increment/decrement amount
- Increase update frequency

**Pattern doesn't stop:**
- Ensure flag reset in `allLedsOff()`
- Check `if (!isPattern) return;` at function start

**Conflicts with other patterns:**
- Only one animation should be active
- `allLedsOff()` called before starting new pattern

## Testing Checklist

- [ ] Pattern runs smoothly without flickering
- [ ] Serial monitor shows correct status message
- [ ] Pattern stops when switching to other states
- [ ] Works on ESP8266 (if available)
- [ ] Works on ESP32 (if available)
- [ ] HTTP endpoint responds quickly (<100ms)
- [ ] Documentation updated

## Reference

For animation examples and timing: `references/animation-patterns.md` (if available)

For firmware structure: Read `firmware/claude_code_led/claude_code_led.ino`
