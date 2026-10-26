/*
  Plant Watering System — Calibration Sketch
  ESP32 DevKit V1 + Capacitive Soil Moisture Sensor on GPIO34 (ADC1)

  Purpose:
    - Find your sensor's DRY_ADC (in air) and WET_ADC (in water)
    - Prints raw ADC (0-4095) + estimated % every 500ms

  Wiring:
    Sensor VCC -> ESP32 3V3
    Sensor GND -> ESP32 GND
    Sensor AOUT -> ESP32 GPIO34 (ADC1_CH6)

  Usage:
    1. Upload, open Serial Monitor @115200
    2. Note value with sensor in air  -> DRY_ADC
    3. Note value with tip in water   -> WET_ADC
    4. Update DRY_ADC/WET_ADC in main firmware

  Docs: docs/firmware-setup.md, docs/circuit.md
*/

const int SOIL_PIN = 34; // ADC1 — must be 32-39 when Wi-Fi is on

// Temporary guess — replace after measuring!
const int DRY_GUESS = 3800;
const int WET_GUESS = 1600;

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(SOIL_PIN, INPUT);
  Serial.println("\n=== Soil Moisture Calibration ===");
  Serial.println("Sensor AOUT -> GPIO34 | Baud 115200");
  Serial.println("Hold sensor in AIR for dry, dip TIP in WATER for wet.\n");
}

void loop() {
  // 10-sample average to denoise
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(SOIL_PIN);
    delay(10);
  }
  int raw = sum / 10;
  int pct = map(raw, DRY_GUESS, WET_GUESS, 0, 100);
  pct = constrain(pct, 0, 100);

  Serial.print("Raw ADC: ");
  Serial.print(raw);
  Serial.print(" | Est %: ");
  Serial.print(pct);
  Serial.println("%");

  delay(500);
}
