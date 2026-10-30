# Day 02 — Pump, ESP & Relay Found · Blink & LCD Tested · Arduino IDE + Breadboard · Blynk Relay Switch (Working)

**Date:** 2026-10-29 (Wednesday)  
**Focus:** Turn Day 01’s research into a working bench test — verify every building block end-to-end  
**Status:** ✅ Completed  
**Time spent:** ~4–5 hours  
**Video studied today:** [How to make a plant watering system with the Nodemcu ESP8266 board and the new Blynk update — SriTu Hobby](https://www.youtube.com/watch?v=WLCR0r7rYi8) (08:03) + companion article: https://srituhobby.com/how-to-make-a-plant-watering-system-with-the-nodemcu-esp8266-board-and-the-new-blynk-update/

---

## 🎯 Goal for Today

- [x] Review ESP8266 Blynk-update video and map it to our ESP32 build
- [x] Locate/verify pump motor + ESP8266/Relay module + remaining small parts
- [x] Run **Blink test** on ESP to confirm board, USB driver, and port
- [x] Run **LCD display check** (16×2 + I2C module)
- [x] Install & learn **Arduino IDE** (boards, libraries, serial monitor)
- [x] Learn **breadboard & small tools** (rails, jumpers, continuity)
- [x] Learn **Blynk IoT website + mobile app** and wire it as a **relay switch for the pump**
- [x] Verify **pump control via Blynk button is WORKING**
- [x] Document everything in Markdown and update README

---

## ✅ What I Did

### 1. Watched the NodeMCU ESP8266 + New Blynk Video

- **Link:** https://www.youtube.com/watch?v=WLCR0r7rYi8 (+ article above)
- Notes captured in: [`docs/videos/02-nodemcu-esp8266-blynk-update.md`](../docs/videos/02-nodemcu-esp8266-blynk-update.md)
- Key map to our build:
  - `NodeMCU A0` ↔ `ESP32 GPIO34` (soil moisture analog) — single ADC vs multi-ADC lesson
  - `NodeMCU D3` ↔ `ESP32 GPIO26` (relay IN) — same active-LOW logic
  - `NodeMCU D1/D2` ↔ `ESP32 GPIO22/21` (I2C LCD `SCL/SDA`) — same 16×2 + PCF8574 backpack
  - Blynk 2.0 flow confirmed: **Template → Datastreams (`V0` moisture, `V1` pump) → Web Dashboard (Gauge + Button) → Auth Token → `Blynk.begin()` → Mobile sync**

### 2. Found & Verified Hardware

| Item | What was checked | Way to verify |
|------|------------------|---------------|
| **Mini submersible pump motor + pipe** | Located, impeller spins freely, tubing fits nozzle, no kink | Jumpered briefly to separate 5V/2A adapter (not MCU) — water pushed ✅ |
| **Relay module (1-ch, 5V, active-LOW)** | Works on both ESP8266 and ESP32 logic | Powered from `VIN`+`GND`; `IN` to `D3`/`GPIO26`; heard clean click on `HIGH↔LOW` |
| **ESP32 DevKit V1 (and ESP8266 reference from video)** | Confirmed USB-TTL chip (CP2102/CH340) | Port appears in Arduino IDE; blink sketch uploads |
| **LCD 16×2 + I2C backpack** | Address, wiring, contrast | I2C scanner → `0x27` found; `lcd.init()` → “System Loading” → dots animation matches video |
| **Breadboard + jumpers** | Rails understood, continuity OK | VIN/GND rails jumpered; multimeter beep on long rails; center trench for MCU ✅ |
| **9V battery/clip (video) vs our PSU** | Isolation rule kept | Pump on separate supply; MCU GND ↔ PSU GND common; 1N4007 flyback ready |

### 3. Blink Test on ESP

- Installed **CP210x / CH340 driver** → port shows as `COMx` / `/dev/ttyUSB0`
- Arduino IDE → **File → Preferences → Additional Boards Manager URL**:
  ```
  http://arduino.esp8266.com/stable/package_esp8266com_index.json
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- **Boards Manager → esp8266 + esp32 installed** → Selected `NodeMCU 1.0` (ESP8266) and `ESP32 Dev Module`
- Uploaded `File → Examples → 01.Basics → Blink` (built-in LED on GPIO2 / `LED_BUILTIN`):
  ```cpp
  void setup(){ pinMode(LED_BUILTIN, OUTPUT); }
  void loop(){ digitalWrite(LED_BUILTIN, LOW); delay(1000); digitalWrite(LED_BUILTIN, HIGH); delay(1000); }
  ```
- **Result:** ✅ Onboard LED blinks 1 s — board, cable, driver, toolchain all good.

### 4. LCD Display Check

- Wiring: `LCD VCC→VIN`, `GND→GND`, `SDA→D2/GPIO21`, `SCL→D1/GPIO22`
- Ran I2C scanner → `0x27` (some boards `0x3F`) — matches video.
- Test sketch:
  ```cpp
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27,16,2);
  void setup(){ lcd.init(); lcd.backlight(); lcd.print("Hello Plant!"); }
  ```
- **Result:** ✅ LCD shows text + backlight; later ran video’s `System Loading` + dots animation → works.

### 5. Breadboard & Small Tools Learning

- Breadboard anatomy: side power rails (continuous), 5-hole rows (connected per row), center trench isolates MCU sides.
- Jumper types: `M-M` (breadboard-breadboard), `M-F` (MCU→breadboard row), `F-F` (sensor leads) — kept spare 10 pcs.
- Tools: wire stripper for trimming pump leads, multimeter continuity for rail check, electrical tape for pump joint near water.
- Kept pump wires away from sensor AOUT wire to reduce ADC noise.

### 6. Arduino IDE Learned

- Download: https://www.arduino.cc/en/software (2.3.x)
- Verified: **Board selection**, **Port selection**, **Serial Monitor @115200**, **Library Manager** (`Blynk` by Volodymyr Shymanskyy, `LiquidCrystal_I2C`, `ESP8266WiFi`/`WiFi.h`)
- Secrets note: tokens in `firmware/**/secrets.h` (gitignored) — example in `secrets.h.example`

### 7. Blynk IoT Website + Mobile App → Relay Switch → Pump (WORKING)

**Steps we did:**

1. **blynk.cloud → Templates → New Template** → `Plant Watering ESP32` → Hardware ESP32 / ESP8266, WiFi
2. **Datastreams:** `V0 Moisture 0–100` (Gauge) + `V1 Water pump 0/1` (Button/Switch)
3. **Web Dashboard:** Added Gauge + Button → assigned `V0`/`V1` → Save
4. **Devices → Create from Template** → copied `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`
5. **Mobile App → same device → Button on V1 (Switch mode) + Gauge on V0**
6. **Firmware (ESP32 equivalent of video sketch):**
   ```cpp
   #define BLYNK_PRINT Serial
   #include <WiFi.h>
   #include <BlynkSimpleEsp32.h>
   BlynkTimer timer;
   #define sensor 34
   #define waterPump 26
   char auth[] = BLYNK_AUTH_TOKEN; char ssid[] = "..."; char pass[] = "...";
   BLYNK_WRITE(V1){ int r=param.asInt(); digitalWrite(waterPump, r?LOW:HIGH); }
   void soil(){ int v=analogRead(sensor); v=map(v,0,4095,0,100); v=(v-100)*-1; Blynk.virtualWrite(V0,v); }
   ```
7. **Test:**
   - App → **Button ON** → relay clicks → pump runs → LCD shows `Motor is ON`
   - App → **Button OFF** → pump stops → LCD `Motor is OFF`
   - Serial Monitor shows `Moisture : 42%` + `V0` updates on Web Dashboard.

- **Result:** ✅ **End-to-end working** — phone ↔ cloud ↔ ESP ↔ relay ↔ pump.

---

## 🛠️ Materials / Tools Used Today

- **Hardware:** Pump motor + pipe, relay module, ESP32 (and ESP8266 from video for comparison), LCD 16×2 + I2C, breadboard 830-pt, jumpers M-M/M-F, 5V/2A PSU (separate for pump), 9V clip understood (video)
- **Software:** Arduino IDE 2.x, ESP32+ESP8266 board packages, Blynk IoT web + mobile app, I2C scanner sketch, Blink sketch

---

## 🧩 Challenges & Fixes

| Challenge | Fix |
|-----------|-----|
| LCD blank — no I2C | Ran I2C scanner; address `0x27` vs `0x3F` varies; trimmed contrast pot on backpack slightly |
| Relay didn’t click on `HIGH` | Realized relay is active-LOW → `LOW=ON`, use `digitalWrite(relay, LOW)` for pump ON |
| Port not found after switch from ESP8266 to ESP32 | Re-checked Tools → Port; CH340 vs CP2102 driver; swapped to data USB cable |
| Moisture % inverted (dry=0, wet=100) | Added `(value-100)*-1` inversion after `map()` as in SriTu video; verified with dry air vs glass of water |
| Pump not spinning from 9V battery clip | 9V sags under motor surge; moved to regulated 5V 2A adapter + common GND → reliable |

---

## 💡 Learning / Key Takeaways

1. **Blink is Day 1’s “hello world” for hardware** — proves toolchain before chasing sensors.
2. **LCD+ I2C is 4 wires only** — VCC/VIN, GND, SDA, SCL — scanner is mandatory to pick `0x27`/`0x3F`.
3. **Blynk button as relay driver is the simplest IoT pattern** — `BLYNK_WRITE(V1)` → `digitalWrite(relay, state)` — auto/threshold later layers on top.
4. **Separation of power keeps it reliable** — MCU on USB, pump on own 5V with common GND + flyback diode.

---

## ➡️ Next Steps (Day 03)

- [ ] Add auto-mode with threshold + hysteresis (e.g., `<30%` ON, `>60%` OFF) — keep manual Blynk override
- [ ] Calibrate capacitive sensor in real soil (dry vs watered) and record `DRY_ADC/WET_ADC` in `docs/bill-of-materials.md` table
- [ ] Photo set: breadboard + LCD `Motor is ON/OFF` + app button + pump in bottle → `docs/images/day-02-*.jpg`
- [ ] Push Day 03 log and update `progress/README.md` + `README.md` roadmap

---

## ⏱️ Time Spent

| Activity | Hours |
|----------|-------|
| Video study (ESP8266 Blynk update) + notes | 1.0 h |
| Hardware locate & pump/relay check | 0.5 h |
| Blink + LCD verification | 1.0 h |
| Arduino IDE + breadboard & small tools | 1.0 h |
| Blynk web + app + relay→pump integration & test | 1.5 h |
| Docs & README updates | 1.0 h |
| **Total** | **~6 h** |

---

## 🖼️ Media

- 📸 *To add:* `docs/images/day-02-pump-motor.jpg` — pump + pipe on bench
- 📸 *To add:* `docs/images/day-02-blink-test.jpg` — ESP onboard LED + IDE upload success
- 📸 *To add:* `docs/images/day-02-lcd-check.jpg` — LCD showing `System Loading....` → `Moisture: 42%`
- 📸 *To add:* `docs/images/day-02-blynk-button.jpg` — mobile app Button V1 ON/OFF + Web Dashboard Gauge V0
- 📝 *Serial log snippet:* `Moisture: 38% | Relay: ON | LCD: Motor is ON`

---

## 🔗 Commits

- Branch: `arena/360e0168-plant-watering-system`
- Commit: `docs: add day 02 — NodeMCU/ESP8266 video, blink, LCD, Arduino IDE, Blynk relay→pump (working)` (plus this file)

---

*Video deep dive: [`docs/videos/02-nodemcu-esp8266-blynk-update.md`](../docs/videos/02-nodemcu-esp8266-blynk-update.md) · Index: [`README.md`](README.md) · Main README: [`../README.md`](../README.md)*
