/*
 * Claude Code LED Status Indicator
 * 
 * This firmware creates a WiFi-connected LED status indicator
 * that responds to HTTP requests to show Claude Code's working state.
 * 
 * LED States:
 * - Red (D1): Idle - Waiting for input
 * - Yellow (D2): Processing - Claude Code is thinking (with breathing effect)
 * - Yellow (D2): Waiting - Blinking when Claude needs user input
 * - Green (D3): Complete - Task finished successfully
 *
 * API Endpoints:
 * - GET /red or /idle       -> Set to idle state
 * - GET /yellow or /processing -> Set to processing state
 * - GET /waiting or /prompt -> Set to waiting for input state (blinking yellow)
 * - GET /green or /complete -> Set to complete state
 * - GET /status             -> Get current status as JSON
 * 
 * Hardware: ESP8266 (NodeMCU) or ESP32
 */

// ==================== CONFIGURATION ====================

// Uncomment ONE of the following based on your board
#define USE_ESP8266
// #define USE_ESP32

// WiFi Credentials - UPDATE THESE!
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Pin Definitions (adjust if using different pins)
#ifdef USE_ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
  
  const int RED_PIN = D1;      // GPIO5
  const int YELLOW_PIN = D2;   // GPIO4
  const int GREEN_PIN = D3;    // GPIO0
#endif

#ifdef USE_ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  WebServer server(80);
  
  const int RED_PIN = 5;
  const int YELLOW_PIN = 18;
  const int GREEN_PIN = 19;
  
  // PWM settings for ESP32
  const int PWM_CHANNEL = 0;
  const int PWM_FREQ = 5000;
  const int PWM_RESOLUTION = 8;
#endif

// ==================== GLOBAL VARIABLES ====================

String currentStatus = "idle";
unsigned long bootTime = 0;

// Breathing effect variables
bool isBreathing = false;
int breathBrightness = 0;
int breathDirection = 5;  // Increment/decrement amount
unsigned long lastBreathUpdate = 0;
const int BREATH_INTERVAL = 20;  // ms between brightness updates

// Blinking effect variables (for waiting state)
bool isBlinking = false;
bool blinkState = false;
unsigned long lastBlinkTime = 0;
const int BLINK_INTERVAL = 500;  // 500ms on/off

// ==================== LED CONTROL FUNCTIONS ====================

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
}

void setLedState(String status) {
  allLedsOff();
  currentStatus = status;

  if (status == "idle" || status == "red") {
    currentStatus = "idle";
    digitalWrite(RED_PIN, HIGH);
    Serial.println("📍 Status: IDLE (Red LED)");
  }
  else if (status == "processing" || status == "yellow") {
    currentStatus = "processing";
    isBreathing = true;
    breathBrightness = 0;
    breathDirection = 5;
    Serial.println("📍 Status: PROCESSING (Yellow LED - Breathing)");
  }
  else if (status == "waiting" || status == "prompt") {
    currentStatus = "waiting";
    isBlinking = true;
    blinkState = false;
    lastBlinkTime = millis();
    Serial.println("📍 Status: WAITING (Yellow LED - Blinking)");
  }
  else if (status == "complete" || status == "green") {
    currentStatus = "complete";
    digitalWrite(GREEN_PIN, HIGH);
    Serial.println("📍 Status: COMPLETE (Green LED)");
  }
}

void updateBreathing() {
  if (!isBreathing) return;

  unsigned long currentTime = millis();
  if (currentTime - lastBreathUpdate < BREATH_INTERVAL) return;

  lastBreathUpdate = currentTime;

  // Update brightness
  breathBrightness += breathDirection;

  // Reverse direction at limits
  if (breathBrightness >= 255) {
    breathBrightness = 255;
    breathDirection = -5;
  }
  else if (breathBrightness <= 0) {
    breathBrightness = 0;
    breathDirection = 5;
  }

  // Apply brightness
  #ifdef USE_ESP8266
    analogWrite(YELLOW_PIN, breathBrightness);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, breathBrightness);
  #endif
}

void updateBlinking() {
  if (!isBlinking) return;

  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime < BLINK_INTERVAL) return;

  lastBlinkTime = currentTime;
  blinkState = !blinkState;

  // Apply blink state (full brightness on/off)
  #ifdef USE_ESP8266
    analogWrite(YELLOW_PIN, blinkState ? 255 : 0);
  #endif
  #ifdef USE_ESP32
    ledcWrite(PWM_CHANNEL, blinkState ? 255 : 0);
  #endif
}

// ==================== HTTP HANDLERS ====================

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Claude Code LED</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;text-align:center;padding:20px;background:#1a1a2e;color:#fff;}";
  html += ".led{width:50px;height:50px;border-radius:50%;display:inline-block;margin:10px;}";
  html += ".red{background:#ff6b6b;box-shadow:0 0 20px #ff6b6b;}";
  html += ".yellow{background:#ffc107;box-shadow:0 0 20px #ffc107;}";
  html += ".green{background:#00e676;box-shadow:0 0 20px #00e676;}";
  html += ".off{background:#333;box-shadow:none;}";
  html += "button{padding:15px 30px;margin:10px;border:none;border-radius:8px;cursor:pointer;font-size:16px;}";
  html += ".btn-red{background:#ff6b6b;color:#fff;}";
  html += ".btn-yellow{background:#ffc107;color:#333;}";
  html += ".btn-green{background:#00e676;color:#333;}";
  html += "</style></head><body>";
  html += "<h1>🔌 Claude Code LED Status</h1>";
  html += "<p>Current: <strong>" + currentStatus + "</strong></p>";
  html += "<div>";
  html += "<div class='led " + String(currentStatus == "idle" ? "red" : "off") + "'></div>";
  html += "<div class='led " + String(currentStatus == "processing" ? "yellow" : "off") + "'></div>";
  html += "<div class='led " + String(currentStatus == "complete" ? "green" : "off") + "'></div>";
  html += "</div>";
  html += "<div>";
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
  server.send(200, "text/plain", "OK - Status: IDLE (Red LED)");
}

void handleYellow() {
  setLedState("processing");
  server.send(200, "text/plain", "OK - Status: PROCESSING (Yellow LED)");
}

void handleGreen() {
  setLedState("complete");
  server.send(200, "text/plain", "OK - Status: COMPLETE (Green LED)");
}

void handleWaiting() {
  setLedState("waiting");
  server.send(200, "text/plain", "OK - Status: WAITING (Yellow LED - Blinking)");
}

void handleStatus() {
  unsigned long uptime = (millis() - bootTime) / 1000;
  
  String json = "{";
  json += "\"status\":\"" + currentStatus + "\",";
  json += "\"uptime\":" + String(uptime) + ",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI());
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
  
  // Blink all LEDs while connecting
  int ledState = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    
    // Cycle through LEDs to show connecting
    allLedsOff();
    switch (ledState % 3) {
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
}

// ==================== WEB SERVER SETUP ====================

void setupWebServer() {
  // Define routes
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

  // Enable CORS
  server.enableCORS(true);

  server.begin();
  Serial.println("🌐 HTTP server started on port 80");
}

// ==================== MAIN SETUP ====================

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(100);
  
  Serial.println();
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║   Claude Code LED Status Indicator        ║");
  Serial.println("║   Version 1.0                             ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize LED pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  
  // ESP32 PWM setup
  #ifdef USE_ESP32
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(YELLOW_PIN, PWM_CHANNEL);
  #endif
  
  // Start with all LEDs off
  allLedsOff();
  
  // Connect to WiFi
  setupWiFi();
  
  // Start web server
  setupWebServer();
  
  // Record boot time
  bootTime = millis();
  
  // Set initial state to idle (red)
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
  Serial.println("/green  ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
}

// ==================== MAIN LOOP ====================

void loop() {
  // Handle HTTP requests
  server.handleClient();

  // Update breathing effect if active
  updateBreathing();

  // Update blinking effect if active
  updateBlinking();

  // Check WiFi connection and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected! Reconnecting...");
    setupWiFi();
  }

  // Small delay to prevent watchdog issues
  delay(1);
}
