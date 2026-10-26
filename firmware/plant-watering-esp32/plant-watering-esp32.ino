/*
  Plant Watering System — ESP32 Main Firmware (Blynk IoT)
  --------------------------------------------------------
  Hardware: ESP32 DevKit V1 + Capacitive Soil Moisture Sensor (GPIO34) + Relay (GPIO26) + Pump (separate PSU)
  Cloud:    Blynk IoT — Virtual Pins V0..V6

  Quick start:
    1. Fill secrets in secrets.h (copy secrets.h.example -> secrets.h)
    2. Calibrate DRY_ADC / WET_ADC via firmware/calibration/calibration.ino
    3. Select Board: ESP32 Dev Module, correct Port, Upload, Serial Monitor @115200

  Docs:
    - docs/circuit.md        — wiring & safety
    - docs/firmware-setup.md — Blynk setup & calibration
    - docs/bill-of-materials.md — parts

  Repo: https://github.com/Dilujan-Uki/Plant-watering-system
*/

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Plant Watering ESP32"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

// Uncomment if you keep secrets in secrets.h instead:
// #include "secrets.h"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ===== Wi-Fi (from secrets.h or hardcode) =====
// char ssid[] = WIFI_SSID;
// char pass[] = WIFI_PASS;
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// ===== Pins =====
const int SOIL_PIN  = 34;  // ADC1 only! (32-39) — sensor AOUT
const int RELAY_PIN = 26;  // relay IN — active-LOW: LOW = pump ON

// Optional pins (uncomment when hardware added):
// #define DHT_PIN 4
// #define OLED_SDA 21
// #define OLED_SCL 22
// #define BTN_PIN 13
// #define FLOAT_PIN 27  // reservoir low switch

// ===== Calibration — update after calibration sketch =====
const int DRY_ADC = 3800; // sensor in air
const int WET_ADC = 1600; // sensor in water

// ===== Hysteresis thresholds (%) =====
const int MOISTURE_ON  = 30; // below 30% -> pump ON
const int MOISTURE_OFF = 60; // above 60% -> pump OFF

// ===== Safety =====
const unsigned long PUMP_MAX_MS   = 10000; // max 10 s burst
const unsigned long PUMP_COOLDOWN = 30000; // 30 s between bursts

// ===== State =====
BlynkTimer timer;
int  moisturePct = 0;
bool pumpState   = false; // false = OFF (relay HIGH)
unsigned long pumpStart   = 0;
unsigned long lastPumpOff = 0;

// ===== Helpers =====
int readMoisturePct() {
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(SOIL_PIN); delay(10); }
  int raw = sum / 10;
  int pct = map(raw, DRY_ADC, WET_ADC, 0, 100);
  return constrain(pct, 0, 100);
}

void pumpOn(const char* reason) {
  if (lastPumpOff != 0 && millis() - lastPumpOff < PUMP_COOLDOWN) {
    Serial.println("Pump ON blocked: cooldown");
    return;
  }
  digitalWrite(RELAY_PIN, LOW); // active-LOW
  pumpState = true;
  pumpStart = millis();
  Serial.print("[PUMP] ON (");
  Serial.print(reason);
  Serial.println(")");
  Blynk.virtualWrite(V3, 1);
  // Blynk.logEvent("pump_on", String("Moisture ") + moisturePct + "%");
}

void pumpOff() {
  digitalWrite(RELAY_PIN, HIGH);
  pumpState = false;
  lastPumpOff = millis();
  Serial.println("[PUMP] OFF");
  Blynk.virtualWrite(V3, 0);
}

void checkSoil() {
  moisturePct = readMoisturePct();
  Serial.print("[SOIL] Moisture: ");
  Serial.print(moisturePct);
  Serial.println("%");
  Blynk.virtualWrite(V2, moisturePct);

  // Safety: max burst time
  if (pumpState && millis() - pumpStart > PUMP_MAX_MS) {
    pumpOff();
    Serial.println("[SAFETY] Auto OFF: max burst reached");
    return;
  }

  // Auto logic with hysteresis
  if (!pumpState && moisturePct < MOISTURE_ON) {
    pumpOn("Auto dry");
  } else if (pumpState && moisturePct > MOISTURE_OFF) {
    pumpOff();
  }
}

// Blynk manual override — Button on V5
BLYNK_WRITE(V5) {
  int val = param.asInt();
  if (val == 1) pumpOn("Manual Blynk");
  else          pumpOff();
}

// Optional: sync mode switch V4, slider for threshold, etc.
// BLYNK_WRITE(V4) { /* 0=Auto 1=Manual */ }

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V5);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== Plant Watering System — ESP32 ===");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // start OFF (active-LOW)

  // Optional: pinMode(BTN_PIN, INPUT_PULLUP);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, checkSoil);

  Serial.println("Setup done. Check Blynk app V2 (moisture) & V3 (pump LED).");
}

void loop() {
  Blynk.run();
  timer.run();
}
