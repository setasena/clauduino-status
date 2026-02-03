/*
 * Claude Code LED Status Indicator
 *
 * This firmware creates a WiFi-connected LED status indicator
 * that responds to HTTP requests to show Claude Code's working state.
 *
 * LED States:
 * - Red: Idle - Waiting for input
 * - Yellow: Processing - Claude Code is thinking (with breathing effect)
 * - Yellow: Waiting - Blinking when Claude needs user input
 * - Green: Complete - Task finished successfully
 *
 * API Endpoints:
 * - GET /red or /idle       -> Set to idle state
 * - GET /yellow or /processing -> Set to processing state
 * - GET /waiting or /prompt -> Set to waiting for input state (blinking)
 * - GET /green or /complete -> Set to complete state
 * - GET /status             -> Get current status as JSON
 *
 * Hardware: ESP8266 (NodeMCU) or ESP32
 *
 * Supports two LED configurations:
 * 1. Three separate LEDs (Red, Yellow, Green)
 * 2. Single RGB LED (Common Cathode) - mixes Red+Green for Yellow
 *
 * Optional: Active buzzer for audio feedback
 */

// ==================== CONFIGURATION ====================

// Board Selection - Uncomment ONE
#define USE_ESP8266
// #define USE_ESP32

// LED Configuration - Uncomment ONE
// #define USE_SEPARATE_LEDS    // Three separate LEDs (Red, Yellow, Green)
#define USE_RGB_LED             // Single RGB LED (Common Cathode)

// Optional Features
#define USE_BUZZER              // Comment out to disable buzzer

// WiFi Credentials - UPDATE THESE!
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ==================== PIN DEFINITIONS ====================

#ifdef USE_ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);

  #ifdef USE_SEPARATE_LEDS
    // Three separate LEDs
    const int RED_PIN = D1;      // GPIO5
    const int YELLOW_PIN = D2;   // GPIO4
    const int GREEN_PIN = D3;    // GPIO0
  #endif

  #ifdef USE_RGB_LED
    // RGB LED (Common Cathode) - accent colors via mixing
    const int RGB_RED_PIN = D1;    // GPIO5 - Red leg
    const int RGB_GREEN_PIN = D2;  // GPIO4 - Green leg
    // Blue leg not used
  #endif

  #ifdef USE_BUZZER
    const int BUZZER_PIN = D5;     // GPIO14
  #endif
#endif

#ifdef USE_ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  WebServer server(80);

  #ifdef USE_SEPARATE_LEDS
    const int RED_PIN = 5;
    const int YELLOW_PIN = 18;
    const int GREEN_PIN = 19;
  #endif

  #ifdef USE_RGB_LED
    const int RGB_RED_PIN = 5;
    const int RGB_GREEN_PIN = 18;
  #endif

  #ifdef USE_BUZZER
    const int BUZZER_PIN = 14;
  #endif

  // PWM settings for ESP32
  const int PWM_CHANNEL_0 = 0;
  const int PWM_CHANNEL_1 = 1;
  const int PWM_FREQ = 5000;
  const int PWM_RESOLUTION = 8;
#endif

// ==================== GLOBAL VARIABLES ====================

String currentStatus = "idle";
unsigned long bootTime = 0;

// Breathing effect variables
bool isBreathing = false;
int breathBrightness = 0;
int breathDirection = 5;
unsigned long lastBreathUpdate = 0;
const int BREATH_INTERVAL = 20;

// Blinking effect variables
bool isBlinking = false;
bool blinkState = false;
unsigned long lastBlinkTime = 0;
const int BLINK_INTERVAL = 500;

#ifdef USE_BUZZER
  unsigned long buzzerStartTime = 0;
  bool buzzerActive = false;
  int buzzerBeepCount = 0;
  int buzzerBeepsTarget = 0;
  bool buzzerBeepState = false;
  const int BUZZER_BEEP_DURATION = 100;
  const int BUZZER_BEEP_GAP = 100;
#endif

// ==================== BUZZER FUNCTIONS ====================

#ifdef USE_BUZZER
void buzzerOn() {
  digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff() {
  digitalWrite(BUZZER_PIN, LOW);
}

void startBeeps(int count) {
  buzzerBeepCount = 0;
  buzzerBeepsTarget = count;
  buzzerActive = true;
  buzzerBeepState = true;
  buzzerStartTime = millis();
  buzzerOn();
}

void updateBuzzer() {
  if (!buzzerActive) return;

  unsigned long elapsed = millis() - buzzerStartTime;

  if (buzzerBeepState) {
    if (elapsed >= BUZZER_BEEP_DURATION) {
      buzzerOff();
      buzzerBeepState = false;
      buzzerStartTime = millis();
      buzzerBeepCount++;
      if (buzzerBeepCount >= buzzerBeepsTarget) {
        buzzerActive = false;
      }
    }
  } else {
    if (elapsed >= BUZZER_BEEP_GAP) {
      buzzerOn();
      buzzerBeepState = true;
      buzzerStartTime = millis();
    }
  }
}
#endif

// ==================== LED CONTROL FUNCTIONS ====================

void setYellowBrightness(int brightness) {
  #ifdef USE_SEPARATE_LEDS
    #ifdef USE_ESP8266
      analogWrite(YELLOW_PIN, brightness);
    #endif
    #ifdef USE_ESP32
      ledcWrite(PWM_CHANNEL_0, brightness);
    #endif
  #endif

  #ifdef USE_RGB_LED
    #ifdef USE_ESP8266
      analogWrite(RGB_RED_PIN, brightness);
      analogWrite(RGB_GREEN_PIN, brightness);
    #endif
    #ifdef USE_ESP32
      ledcWrite(PWM_CHANNEL_0, brightness);
      ledcWrite(PWM_CHANNEL_1, brightness);
    #endif
  #endif
}

void allLedsOff() {
  #ifdef USE_SEPARATE_LEDS
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    #ifdef USE_ESP8266
      analogWrite(YELLOW_PIN, 0);
    #endif
    #ifdef USE_ESP32
      ledcWrite(PWM_CHANNEL_0, 0);
    #endif
  #endif

  #ifdef USE_RGB_LED
    #ifdef USE_ESP8266
      analogWrite(RGB_RED_PIN, 0);
      analogWrite(RGB_GREEN_PIN, 0);
    #endif
    #ifdef USE_ESP32
      ledcWrite(PWM_CHANNEL_0, 0);
      ledcWrite(PWM_CHANNEL_1, 0);
    #endif
  #endif

  isBreathing = false;
  isBlinking = false;
}

void setRedLed() {
  #ifdef USE_SEPARATE_LEDS
    digitalWrite(RED_PIN, HIGH);
  #endif
  #ifdef USE_RGB_LED
    #ifdef USE_ESP8266
      analogWrite(RGB_RED_PIN, 255);
      analogWrite(RGB_GREEN_PIN, 0);
    #endif
    #ifdef USE_ESP32
      ledcWrite(PWM_CHANNEL_0, 255);
      ledcWrite(PWM_CHANNEL_1, 0);
    #endif
  #endif
}

void setGreenLed() {
  #ifdef USE_SEPARATE_LEDS
    digitalWrite(GREEN_PIN, HIGH);
  #endif
  #ifdef USE_RGB_LED
    #ifdef USE_ESP8266
      analogWrite(RGB_RED_PIN, 0);
      analogWrite(RGB_GREEN_PIN, 255);
    #endif
    #ifdef USE_ESP32
      ledcWrite(PWM_CHANNEL_0, 0);
      ledcWrite(PWM_CHANNEL_1, 255);
    #endif
  #endif
}

void setLedState(String status) {
  allLedsOff();
  currentStatus = status;

  if (status == "idle" || status == "red") {
    currentStatus = "idle";
    setRedLed();
    Serial.println("📍 Status: IDLE (Red)");
  }
  else if (status == "processing" || status == "yellow") {
    currentStatus = "processing";
    isBreathing = true;
    breathBrightness = 0;
    breathDirection = 5;
    Serial.println("📍 Status: PROCESSING (Yellow - Breathing)");
  }
  else if (status == "waiting" || status == "prompt") {
    currentStatus = "waiting";
    isBlinking = true;
    blinkState = false;
    lastBlinkTime = millis();
    #ifdef USE_BUZZER
      startBeeps(3);
    #endif
    Serial.println("📍 Status: WAITING (Yellow - Blinking)");
  }
  else if (status == "complete" || status == "green") {
    currentStatus = "complete";
    setGreenLed();
    #ifdef USE_BUZZER
      startBeeps(2);
    #endif
    Serial.println("📍 Status: COMPLETE (Green)");
  }
}

void updateBreathing() {
  if (!isBreathing) return;

  unsigned long currentTime = millis();
  if (currentTime - lastBreathUpdate < BREATH_INTERVAL) return;

  lastBreathUpdate = currentTime;
  breathBrightness += breathDirection;

  if (breathBrightness >= 255) {
    breathBrightness = 255;
    breathDirection = -5;
  }
  else if (breathBrightness <= 0) {
    breathBrightness = 0;
    breathDirection = 5;
  }

  setYellowBrightness(breathBrightness);
}

void updateBlinking() {
  if (!isBlinking) return;

  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime < BLINK_INTERVAL) return;

  lastBlinkTime = currentTime;
  blinkState = !blinkState;
  setYellowBrightness(blinkState ? 255 : 0);
}

// ==================== HTTP HANDLERS ====================

void handleRoot() {
  String ledType = "3 Separate LEDs";
  #ifdef USE_RGB_LED
    ledType = "RGB LED";
  #endif

  String buzzerStatus = "No";
  #ifdef USE_BUZZER
    buzzerStatus = "Yes";
  #endif

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Claude Code LED</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;text-align:center;padding:20px;background:#1a1a2e;color:#fff;}";
  html += ".led{width:60px;height:60px;border-radius:50%;display:inline-block;margin:10px;transition:all 0.3s;}";
  html += ".red{background:#ff6b6b;box-shadow:0 0 30px #ff6b6b;}";
  html += ".yellow{background:#ffc107;box-shadow:0 0 30px #ffc107;}";
  html += ".green{background:#00e676;box-shadow:0 0 30px #00e676;}";
  html += ".off{background:#333;box-shadow:none;}";
  html += "button{padding:15px 30px;margin:10px;border:none;border-radius:8px;cursor:pointer;font-size:16px;}";
  html += ".btn-red{background:#ff6b6b;color:#fff;}";
  html += ".btn-yellow{background:#ffc107;color:#333;}";
  html += ".btn-green{background:#00e676;color:#333;}";
  html += "h2{color:#888;font-weight:normal;font-size:14px;}";
  html += "</style></head><body>";
  html += "<h1>🔌 Claude Code LED Status</h1>";
  html += "<h2>" + ledType + " | Buzzer: " + buzzerStatus + "</h2>";
  html += "<p>Current: <strong>" + currentStatus + "</strong></p>";
  html += "<div>";

  String ledClass = "off";
  if (currentStatus == "idle") ledClass = "red";
  else if (currentStatus == "processing" || currentStatus == "waiting") ledClass = "yellow";
  else if (currentStatus == "complete") ledClass = "green";
  html += "<div class='led " + ledClass + "'></div>";

  html += "</div>";
  html += "<div style='margin-top:20px;'>";
  html += "<button class='btn-red' onclick=\"fetch('/red')\">Idle</button>";
  html += "<button class='btn-yellow' onclick=\"fetch('/yellow')\">Processing</button>";
  html += "<button class='btn-yellow' onclick=\"fetch('/waiting')\">Waiting</button>";
  html += "<button class='btn-green' onclick=\"fetch('/green')\">Complete</button>";
  html += "</div>";
  html += "<p style='margin-top:30px;color:#888;'>IP: " + WiFi.localIP().toString() + "</p>";
  html += "<script>setInterval(()=>location.reload(),5000);</script>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleRed() {
  setLedState("idle");
  server.send(200, "text/plain", "OK - Status: IDLE (Red)");
}

void handleYellow() {
  setLedState("processing");
  server.send(200, "text/plain", "OK - Status: PROCESSING (Yellow - Breathing)");
}

void handleGreen() {
  setLedState("complete");
  server.send(200, "text/plain", "OK - Status: COMPLETE (Green)");
}

void handleWaiting() {
  setLedState("waiting");
  server.send(200, "text/plain", "OK - Status: WAITING (Yellow - Blinking)");
}

void handleStatus() {
  unsigned long uptime = (millis() - bootTime) / 1000;

  String ledType = "separate";
  #ifdef USE_RGB_LED
    ledType = "rgb";
  #endif

  String json = "{";
  json += "\"status\":\"" + currentStatus + "\",";
  json += "\"uptime\":" + String(uptime) + ",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"led_type\":\"" + ledType + "\",";
  #ifdef USE_BUZZER
    json += "\"buzzer\":true";
  #else
    json += "\"buzzer\":false";
  #endif
  json += "}";

  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

// ==================== WIFI SETUP ====================

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int ledState = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);

    allLedsOff();
    switch (ledState % 3) {
      case 0: setRedLed(); break;
      case 1: setYellowBrightness(255); break;
      case 2: setGreenLed(); break;
    }
    ledState++;

    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ WiFi Connected!");
  Serial.print("📍 IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("📶 Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  #ifdef USE_BUZZER
    startBeeps(1);
  #endif
}

// ==================== WEB SERVER SETUP ====================

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/red", handleRed);
  server.on("/idle", handleRed);
  server.on("/yellow", handleYellow);
  server.on("/processing", handleYellow);
  server.on("/waiting", handleWaiting);
  server.on("/prompt", handleWaiting);
  server.on("/green", handleGreen);
  server.on("/complete", handleGreen);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);

  server.enableCORS(true);
  server.begin();
  Serial.println("🌐 HTTP server started on port 80");
}

// ==================== MAIN SETUP ====================

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║   Claude Code LED Status Indicator        ║");
  #ifdef USE_RGB_LED
    Serial.println("║   Hardware: RGB LED (Common Cathode)      ║");
  #else
    Serial.println("║   Hardware: 3 Separate LEDs               ║");
  #endif
  #ifdef USE_BUZZER
    Serial.println("║   Buzzer: Enabled                         ║");
  #else
    Serial.println("║   Buzzer: Disabled                        ║");
  #endif
  Serial.println("║   Version 2.0                             ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();

  // Initialize LED pins
  #ifdef USE_SEPARATE_LEDS
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    #ifdef USE_ESP32
      ledcSetup(PWM_CHANNEL_0, PWM_FREQ, PWM_RESOLUTION);
      ledcAttachPin(YELLOW_PIN, PWM_CHANNEL_0);
    #endif
  #endif

  #ifdef USE_RGB_LED
    pinMode(RGB_RED_PIN, OUTPUT);
    pinMode(RGB_GREEN_PIN, OUTPUT);
    #ifdef USE_ESP32
      ledcSetup(PWM_CHANNEL_0, PWM_FREQ, PWM_RESOLUTION);
      ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
      ledcAttachPin(RGB_RED_PIN, PWM_CHANNEL_0);
      ledcAttachPin(RGB_GREEN_PIN, PWM_CHANNEL_1);
    #endif
  #endif

  #ifdef USE_BUZZER
    pinMode(BUZZER_PIN, OUTPUT);
    buzzerOff();
    Serial.println("🔊 Testing buzzer...");
    buzzerOn();
    delay(100);
    buzzerOff();
  #endif

  allLedsOff();
  setupWiFi();
  setupWebServer();

  bootTime = millis();
  setLedState("idle");

  Serial.println();
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║   Ready! Use these endpoints:             ║");
  Serial.print("║   http://");
  Serial.print(WiFi.localIP());
  String padding = "";
  for (int i = WiFi.localIP().toString().length(); i < 15; i++) padding += " ";
  Serial.print(padding);
  Serial.println("/red    ║");
  Serial.print("║   http://");
  Serial.print(WiFi.localIP());
  Serial.print(padding);
  Serial.println("/yellow ║");
  Serial.print("║   http://");
  Serial.print(WiFi.localIP());
  Serial.print(padding);
  Serial.println("/waiting║");
  Serial.print("║   http://");
  Serial.print(WiFi.localIP());
  Serial.print(padding);
  Serial.println("/green  ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
}

// ==================== MAIN LOOP ====================

void loop() {
  server.handleClient();
  updateBreathing();
  updateBlinking();

  #ifdef USE_BUZZER
    updateBuzzer();
  #endif

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected! Reconnecting...");
    setupWiFi();
  }

  delay(1);
}
