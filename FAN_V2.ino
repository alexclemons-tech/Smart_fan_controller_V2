#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials for the ESP32-C3 access point.
// No password is used, per request.
const char* AP_SSID = "FanControllerV2";

// Pins: adjust these to match your wiring.
// Only mic, thermostat/temp, and PWM are used.
const int MIC_PIN = 2;
const int TEMP_PIN = 3;
const int PWM_PIN = 4;

// PWM configuration for the fan.
const int PWM_CHANNEL = 0;
const int PWM_FREQ = 25000;
const int PWM_RESOLUTION = 8;
const int PWM_MAX = 255;

WebServer server(80);

struct FanState {
  int micLevel;
  int tempRaw;
  int fanDuty;
  bool autoMode;
};

FanState state = {0, 0, 0, true};
unsigned long lastSampleMs = 0;
const unsigned long SAMPLE_INTERVAL_MS = 250;

String renderPage() {
  String html;
  html.reserve(2500);
  html += F("<!DOCTYPE html><html><head><meta charset='utf-8'>");
  html += F("<meta http-equiv='refresh' content='1'>");
  html += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  html += F("<title>Fan Controller V2</title>");
  html += F("<style>");
  html += F("body{font-family:Arial,sans-serif;background:#101418;color:#e8eef2;margin:0;padding:20px;}");
  html += F(".card{max-width:720px;margin:0 auto;background:#182028;border:1px solid #26303a;border-radius:16px;padding:20px;}");
  html += F("h1{margin-top:0;font-size:28px;} .row{display:flex;justify-content:space-between;gap:12px;padding:10px 0;border-bottom:1px solid #26303a;}");
  html += F(".row:last-child{border-bottom:none;} .label{font-weight:bold;} .value{font-variant-numeric:tabular-nums;}");
  html += F(".badge{display:inline-block;padding:6px 10px;border-radius:999px;background:#274; color:#dff5e1;font-size:12px;}");
  html += F(".muted{color:#a9b4be;font-size:13px;}");
  html += F("</style></head><body><div class='card'>");
  html += F("<h1>Fan Controller V2</h1>");
  html += F("<div class='badge'>Live refresh every 1 second</div>");
  html += F("<p class='muted'>Web-based control/status for mic, thermostat, and PWM fan.</p>");
  html += F("<div class='row'><div class='label'>Mode</div><div class='value'>");
  html += state.autoMode ? F("Automatic") : F("Manual");
  html += F("</div></div>");
  html += F("<div class='row'><div class='label'>Microphone level</div><div class='value'>");
  html += String(state.micLevel);
  html += F("</div></div>");
  html += F("<div class='row'><div class='label'>Temperature raw</div><div class='value'>");
  html += String(state.tempRaw);
  html += F("</div></div>");
  html += F("<div class='row'><div class='label'>Fan PWM duty</div><div class='value'>");
  html += String(state.fanDuty);
  html += F(" / 255</div></div>");
  html += F("<div class='row'><div class='label'>AP SSID</div><div class='value'>");
  html += AP_SSID;
  html += F("</div></div>");
  html += F("<p class='muted'>This page refreshes automatically every second.</p>");
  html += F("</div></body></html>");
  return html;
}

int readAveragedAnalog(int pin) {
  long total = 0;
  for (int i = 0; i < 8; i++) {
    total += analogRead(pin);
    delay(2);
  }
  return total / 8;
}

void updateControlLoop() {
  int mic = readAveragedAnalog(MIC_PIN);
  int temp = readAveragedAnalog(TEMP_PIN);

  state.micLevel = mic;
  state.tempRaw = temp;

  // Simple combined control strategy:
  // - temperature drives the baseline fan speed
  // - microphone activity boosts the fan a bit for noise/airflow response
  int tempContribution = map(constrain(temp, 0, 4095), 0, 4095, 0, 200);
  int micContribution = map(constrain(mic, 0, 4095), 0, 4095, 0, 55);

  int duty = tempContribution + micContribution;
  duty = constrain(duty, 0, PWM_MAX);
  state.fanDuty = duty;

  analogWrite(PWM_PIN, duty);
}

void handleRoot() {
  server.send(200, "text/html", renderPage());
}

void handleStatus() {
  String json = "{";
  json += "\"micLevel\":" + String(state.micLevel) + ",";
  json += "\"tempRaw\":" + String(state.tempRaw) + ",";
  json += "\"fanDuty\":" + String(state.fanDuty) + ",";
  json += "\"autoMode\":" + String(state.autoMode ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(MIC_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
  pinMode(PWM_PIN, OUTPUT);

  analogWriteResolution(PWM_RESOLUTION);
  analogWriteFrequency(PWM_PIN, PWM_FREQ);
  analogWrite(PWM_PIN, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID);

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = millis();
    updateControlLoop();
  }
}
