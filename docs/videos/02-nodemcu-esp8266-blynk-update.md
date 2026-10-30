# Video Study — NodeMCU ESP8266 + New Blynk Update Plant Watering System

> **Video:** [How to make a plant watering system with the Nodemcu ESP8266 board and the new Blynk update](https://www.youtube.com/watch?v=WLCR0r7rYi8) — SriTu Hobby  
> **Date studied:** 2026-10-29 (Day 02)  
> **Length:** 08:03 · **Views:** 292k · **Likes:** 3.9k  
> **Companion article (code + wiring):** https://srituhobby.com/how-to-make-a-plant-watering-system-with-the-nodemcu-esp8266-board-and-the-new-blynk-update/  
> **Related earlier video:** [ESP32 + Blynk build](https://www.youtube.com/watch?v=Ix3a3ThsHfA) — same channel, Day 01

---

## 1. Why this video?

After Day 01’s ESP32 research, this video was chosen to:

1. See the **ESP8266 variant** (cheaper, single ADC, still Wi-Fi-capable) — useful comparison to the ESP32 dev board already gathered.
2. Learn the **new Blynk 2.0 (Blynk.cloud) workflow** — Template → Datastreams → Web Dashboard → Mobile App, which replaces legacy Blynk.
3. Understand **pump + relay as a Blynk switch** — the core of yesterday’s hands-on test (relay as on/off switch for pump controlled from phone).

---

## 2. Components Used in the Video

| # | Component | Role in the video | Our status (Day 02) |
|---|-----------|-------------------|---------------------|
| 1 | **NodeMCU ESP8266 (CP2102)** | Main MCU + Wi-Fi; reads soil sensor, drives relay, talks to Blynk | ✅ Understood — also have ESP32 DevKit; ESP8266 is pin-compatible conceptually, good fallback board |
| 2 | **Soil moisture sensor** | Analog moisture → ESP8266 `A0` | ✅ Gathered Day 01 (capacitive) — video uses same placement on breadboard |
| 3 | **5V 1-Channel Relay module** | Switch for pump; controlled from Blynk `V1` button | ✅ **Found & tested** — Blynk switch → relay click → pump ON verified |
| 4 | **16×2 LCD + I2C module (PCF8574)** | Local display: `Moisture: 42%` + `Motor is ON/OFF` | ✅ **Display check done** — LCD init + I2C scan (addr `0x27` vs `0x3F`) working |
| 5 | **Breadboard (830-pt) + Jumper wires (M-M, M-F)** | Prototyping without soldering | ✅ **Learned** — power rails, center trench, VIN/GND rows |
| 6 | **Mini DC water pump + mini pipe** | Actuator that moves water to plant | ✅ **Found pump motor** — tested with separate 9V/5V supply (not from MCU) |
| 7 | **9V battery + clip** | Pump supply (video’s demo supply) | ✅ Understood isolation — we use 5V/2A adapter for our pump (safer than 9V for 5V pump) |
| 8 | Micro-USB data cable | Flash NodeMCU via Arduino IDE | ✅ Have |

> SriTu Hobby’s 9V battery is for demo; for prolonged use a **regulated 5V 2A adapter** (or 18650 + buck) is more stable. Rule unchanged: **never power pump from ESP8266/ESP32 5V pin**.

---

## 3. Wiring Learned (NodeMCU ESP8266 mapping)

The video’s circuit translates directly to ESP32 (pin names differ, idea same):

| Sensor / Module | NodeMCU Pin (video) | ESP32 Equivalent (our build) | Notes |
|-----------------|---------------------|------------------------------|-------|
| Soil sensor VCC | `3V3` | `3V3` | Sensor power |
| Soil sensor GND | `GND` | `GND` | Common ground |
| Soil sensor A0 | `A0` (only ADC on ESP8266) | `GPIO34` (ADC1) | ESP8266 has **one ADC (A0, 0–1V scaled)**; ESP32 has many ADC1 pins — prefer ADC1 when Wi-Fi on |
| I2C LCD VCC | `VIN` (5V) | `VIN` | LCD backlight needs 5V |
| I2C LCD GND | `GND` | `GND` | — |
| I2C SDA | `D2` (GPIO4) | `GPIO21` | Default I2C bus |
| I2C SCL | `D1` (GPIO5) | `GPIO22` | — |
| Relay VCC | `VIN` | `VIN` | 5V coil |
| Relay GND | `GND` | `GND` | — |
| Relay IN | `D3` (GPIO0) | `GPIO26` | Active-LOW: `LOW = ON` on most modules |
| Pump + → Relay **NO** | via Relay **NO/COM** | same | **NO = safe OFF by default**; COM → PSU +, NO → pump + |
| Pump − → PSU − | PSU − → common GND | same | PSU GND tied to MCU GND |

**Breadboard tip learned:** NodeMCU spans the center trench; its `VIN`/`GND` rows are then distributed along the side power rails for sensor/LCD/relay to tap.

---

## 4. Code & Blynk Workflow (New Blynk Update)

The video’s sketch (condensed, full in article):

```cpp
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char auth[] = ""; // BLYNK_AUTH_TOKEN
char ssid[] = ""; // WiFi
char pass[] = "";

BlynkTimer timer;
bool Relay = 0;
#define sensor A0
#define waterPump D3

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH); // OFF (active-LOW)
  lcd.init(); lcd.backlight();
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, soilMoistureSensor);
}

BLYNK_WRITE(V1) { // Button widget on V1
  Relay = param.asInt();
  if (Relay == 1) { digitalWrite(waterPump, LOW); lcd.setCursor(0,1); lcd.print("Motor is ON "); }
  else            { digitalWrite(waterPump, HIGH);lcd.setCursor(0,1); lcd.print("Motor is OFF"); }
}

void soilMoistureSensor() {
  int value = analogRead(sensor); // 0..1024 on ESP8266
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1; // invert (dry=high ADC)
  Blynk.virtualWrite(V0, value);
  lcd.setCursor(0,0); lcd.print("Moisture :"); lcd.print(value); lcd.print(" ");
}
void loop() { Blynk.run(); timer.run(); }
```

**Blynk 2.0 steps we replicated:**

1. **blynk.cloud → Templates → New Template** → Name, Hardware: ESP8266 (or ESP32), Connection: WiFi
2. **Datastreams:** `V0 Moisture (0–100)` + `V1 Water pump (0/1)`
3. **Web Dashboard:** Gauge on `V0` + Button (Switch mode) on `V1`
4. **Devices → New Device from Template** → copy `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`
5. **Mobile App → add same widgets → same V pins** → test button → relay clicks → pump runs.

Our Day 02 test used **ESP32 equivalent**: `V0→moisture, V1→pump**` — same principle, different pins.

---

## 5. What we verified from this video on Day 02

| Test | Result |
|------|--------|
| Found pump motor + ESP8266-compatible relay + other spares | ✅ Pump identified, separate PSU rule respected |
| Blink test on ESP (built-in LED) | ✅ Uploaded `Blink` via Arduino IDE → LED on GPIO2 (D4) blinks — board + USB driver + port OK |
| LCD display check | ✅ I2C scan found `0x27`; `lcd.init()` + `lcd.print("Hello")` shows; backlight working |
| Breadboard + small tools | ✅ Learned rail continuity, jumper types, wire stripper, multimeter continuity |
| Arduino IDE | ✅ Installed 2.x, added ESP8266/ESP32 board URLs, installed `Blynk`, `LiquidCrystal_I2C`, `ESP8266WiFi` libs |
| Blynk IoT website + Mobile app → relay switch → pump | ✅ **Working end-to-end** — Blynk button (V1) → relay LOW → pump ON; button OFF → pump OFF; LCD mirrors `Motor is ON/OFF` |

---

## 6. Takeaways & Next

- **ESP8266 vs ESP32 for this project:** ESP8266 is cheaper and fine for single-sensor + single-pump. ESP32 wins for multiple ADC sensors, Bluetooth, and dual-core headroom. Code ports with tiny pin changes — we’ll keep **ESP32 as primary**, ESP8266 as proven fallback.
- **Single ADC lesson:** ESP8266 `A0` is the only analog input (0–1V internally, divider to 3.3V on NodeMCU). ESP32’s `GPIO34` is a better choice when you need moisture + battery voltage + light etc.
- **Blynk as switch abstraction:** The video confirms the clean pattern — **Blynk virtual button → `BLYNK_WRITE(Vx)` → `digitalWrite(relay, state)`** — which we already use. Next: add hysteresis auto-mode alongside manual switch.

> Full daily log: [`progress/day-02.md`](../../progress/day-02.md) · References index: [`../references.md`](../references.md)
