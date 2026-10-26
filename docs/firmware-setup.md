# Firmware & Software Setup — ESP32 + Blynk

> From zero to “pump waters when dry, phone shows moisture%” in ~15 minutes.

---

## 1. Prerequisites

- ESP32 DevKit V1 plugged in via **data USB cable**
- [Arduino IDE 2.x](https://www.arduino.cc/en/software) installed
- Wi-Fi credentials (2.4 GHz — ESP32 doesn’t support 5 GHz)
- Blynk account (free): https://blynk.cloud

---

## 2. Install ESP32 Board Support

1. Arduino IDE → **File → Preferences → Additional Boards Manager URLs** → add:

   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

2. **Tools → Board → Boards Manager** → search `esp32` → Install **esp32 by Espressif Systems** (3.x).
3. Select: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
4. **Tools → Port →** choose your COM port / `/dev/ttyUSB0` / `/dev/cu.SLAB*` etc.

If no port appears → install **CP210x** or **CH340** driver (depends on your board’s USB chip).

---

## 3. Install Libraries

**Sketch → Include Library → Manage Libraries** → search & install:

| Library | Author | When needed |
|---------|--------|-------------|
| `Blynk` | Volodymyr Shymanskyy | Always — IoT core |
| `DHT sensor library` | Adafruit | Only if DHT11/22 present |
| `Adafruit Unified Sensor` | Adafruit | DHT dependency |
| `Adafruit SSD1306` + `Adafruit GFX Library` | Adafruit | Only if OLED present |
| `LiquidCrystal_I2C` | Frank de Brabander | Only if 16×2 LCD + I2C present |

---

## 4. Blynk Project Setup (5 min)

### 4.1 Web Console

1. **blynk.cloud → Templates → New Template**
   - Name: `Plant Watering ESP32`
   - Hardware: `ESP32` → Connection: `WiFi`
2. Go to **Datastreams** → Create:

| Virtual Pin | Name | Type | Min/Max | Notes |
|-------------|------|------|---------|-------|
| V0 | Temperature | Double | 0–60 | DHT optional |
| V1 | Humidity | Double | 0–100 | DHT optional |
| V2 | Soil Moisture | Integer | 0–100 | Main gauge, % |
| V3 | Pump Status | Integer | 0/1 | LED widget |
| V4 | Mode Switch | Integer | 0/1 | Switch: 0 Auto / 1 Manual |
| V5 | Manual Pump | Integer | 0/1 | Button (push or switch) |
| V6 | Alert | String | — | Event/notification |

3. **Info →** copy: `BLYNK_TEMPLATE_ID` and `BLYNK_TEMPLATE_NAME`.

### 4.2 Device & Auth Token

- **Devices → New Device → From Template → Plant Watering ESP32**
- Copy **BLYNK_AUTH_TOKEN** (emailed & shown on device page).

### 4.3 Mobile App

- Install **Blynk IoT** (iOS/Android) → log in → your device appears.
- Add widgets bound to V2 (Gauge), V3 (LED), V4 (Switch), V5 (Button).

---

## 5. Calibration Sketch (Do This First!)

Before automation, find *your* sensor’s dry/wet ADC values — they differ by soil, depth, and board.

Create `firmware/calibration/calibration.ino`:

```cpp
const int SOIL_PIN = 34; // ADC1!

void setup() {
  Serial.begin(115200);
  pinMode(SOIL_PIN, INPUT);
}

void loop() {
  int raw = analogRead(SOIL_PIN);
  Serial.print("Raw ADC: ");
  Serial.print(raw);
  Serial.print(" | Est % (guess): ");
  // temporary guess mapping — replace after measuring
  int pct = map(raw, 3800, 1600, 0, 100);
  pct = constrain(pct, 0, 100);
  Serial.print(pct);
  Serial.println("%");
  delay(500);
}
```

**Steps:**

1. Upload → **Tools → Serial Monitor → 115200 baud**
2. **Sensor in air** (dry) → note value → e.g., `DRY = 3800`
3. **Sensor tip in glass of water** (wet, don’t submerge electronics) → note → e.g., `WET = 1600`
4. **Bury in your pot**, water fully, let drain 5 min → observe mid-range
5. Record in table below and paste into main firmware.

| Condition | Raw ADC | Notes |
|-----------|---------|-------|
| In air (dry reference) |  |  |
| In water (wet reference) |  |  |
| Dry soil (plant pot) |  |  |
| Wet soil (just watered) |  |  |

> Re-calibrate whenever you change soil type, sensor depth, or sensor board.

---

## 6. Main Firmware Template

Create `firmware/plant-watering-esp32/plant-watering-esp32.ino`:

```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Plant Watering ESP32"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// === WiFi ===
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// === Pins ===
const int SOIL_PIN  = 34;  // ADC1 — soil AOUT
const int RELAY_PIN = 26;  // active-LOW relay
// Optional:
// const int DHT_PIN = 4;
// const int BTN_PIN = 13;

// === Calibration (from step 5) ===
const int DRY_ADC = 3800;
const int WET_ADC = 1600;

// === Thresholds (hysteresis) ===
const int MOISTURE_ON  = 30; // below 30% → pump ON
const int MOISTURE_OFF = 60; // above 60% → pump OFF

// === Pump safety ===
const unsigned long PUMP_MAX_MS   = 10000; // max 10 s burst
const unsigned long PUMP_COOLDOWN = 30000; // 30 s between bursts

BlynkTimer timer;
int  moisturePct = 0;
bool pumpState   = false; // false = OFF (relay HIGH)
unsigned long pumpStart = 0;
unsigned long lastPumpOff = 0;

// Button from Blynk V5 (manual override)
BLYNK_WRITE(V5) {
  bool manualOn = param.asInt();
  if (manualOn) {
    pumpOn("Manual Blynk");
  } else {
    pumpOff();
  }
}

void pumpOn(const char* reason) {
  if (millis() - lastPumpOff < PUMP_COOLDOWN && lastPumpOff != 0) return;
  digitalWrite(RELAY_PIN, LOW); // active-LOW = ON
  pumpState = true;
  pumpStart = millis();
  Serial.print("Pump ON ("); Serial.print(reason); Serial.println(")");
  Blynk.virtualWrite(V3, 1);
}

void pumpOff() {
  digitalWrite(RELAY_PIN, HIGH); // OFF
  pumpState = false;
  lastPumpOff = millis();
  Serial.println("Pump OFF");
  Blynk.virtualWrite(V3, 0);
}

int readMoisturePct() {
  // 10-sample average to denoise
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(SOIL_PIN); delay(10); }
  int raw = sum / 10;
  int pct = map(raw, DRY_ADC, WET_ADC, 0, 100);
  return constrain(pct, 0, 100);
}

void checkSoil() {
  moisturePct = readMoisturePct();
  Serial.print("Moisture: "); Serial.print(moisturePct); Serial.println("%");
  Blynk.virtualWrite(V2, moisturePct);

  // Safety: max burst
  if (pumpState && millis() - pumpStart > PUMP_MAX_MS) {
    pumpOff();
    Serial.println("Auto OFF: max burst reached");
  }

  // Auto logic (only if not in manual override — read V4 if you want)
  if (!pumpState && moisturePct < MOISTURE_ON) {
    pumpOn("Auto dry");
  } else if (pumpState && moisturePct > MOISTURE_OFF) {
    pumpOff();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // start OFF

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, checkSoil); // check every 2 s
}

void loop() {
  Blynk.run();
  timer.run();
}
```

### 6.1 Upload steps

1. Replace `YOUR_TEMPLATE_ID`, `YOUR_TEMPLATE_NAME`, `YOUR_AUTH_TOKEN`, `YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`.
2. Paste calibrated `DRY_ADC` / `WET_ADC`.
3. **Tools → Board → ESP32 Dev Module**, **Port → your port**, **Upload**.
4. Open Serial Monitor @115200 — you should see `Moisture: 45%` etc.
5. Check Blynk app: V2 gauge moves, V3 LED mirrors pump.

### 6.2 Test matrix

| Test | Expected |
|------|----------|
| Sensor in air | Moisture ~0%, after hysteresis pump ON (if <30%) |
| Sensor in water | Moisture ~100%, pump OFF |
| Sensor in moist soil | Between 30–60%, pump holds last state (hysteresis) |
| Blynk V5 button | Forces pump ON/OFF immediately |
| Pump runs >10 s | Auto OFF (safety cap) + cooldown |

---

## 7. Folder Convention (this repo)

```
firmware/
├── calibration/
│   └── calibration.ino        # raw ADC logger
└── plant-watering-esp32/
    ├── plant-watering-esp32.ino  # main sketch
    └── secrets.h.example          # template for tokens (copy to secrets.h, gitignored)
```

> Never commit `secrets.h` with real tokens — use `secrets.h.example` + `.gitignore` entry.

---

## 8. Next steps after it works

- Add OLED: show `M:42% P:OFF` locally
- Add DHT: publish temp/humidity to V0/V1
- Add float switch: if reservoir LOW → interlock pump + `Blynk.logEvent("low_water")`
- Add deep-sleep for battery: `esp_sleep_enable_timer_wakeup(10*60*1000000ULL)` (wake every 10 min)

---

## Next

- **Circuit:** [`circuit.md`](circuit.md)
- **Log your work:** [`../progress/day-01.md`](../progress/day-01.md)
