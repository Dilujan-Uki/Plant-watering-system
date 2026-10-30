# 🌱 Smart Plant Watering System — ESP32 + IoT

> An open-source IoT-based automatic irrigation system that monitors soil moisture and waters your plants when they need it — remotely monitored via Wi-Fi & Blynk.

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![IoT: Blynk](https://img.shields.io/badge/IoT-Blynk-23CC71.svg)](https://blynk.io/)
[![Status: Complete](https://img.shields.io/badge/Status-Day%203%20%E2%80%94%20Bench%20Prototype%20Complete-brightgreen)]()
[![Made with: Markdown](https://img.shields.io/badge/Docs-Markdown-000000.svg)]()

<p align="center">
  <img src="https://img.shields.io/badge/Focus-Automation%20%7C%20Sustainability%20%7C%20Smart%20Gardening-brightgreen?style=for-the-badge" alt="focus badge"/>
</p>

---

## 📑 Table of Contents

- [About](#-about)
- [What is an Automatic Plant Watering System?](#-what-is-an-automatic-plant-watering-system)
- [What is IoT?](#-what-is-iot)
- [How It Works — System Architecture](#-how-it-works--system-architecture)
- [✨ Features](#-features)
- [🧰 Hardware — Bill of Materials](#-hardware--bill-of-materials)
- [🔧 Tools Required](#-tools-required)
- [🔌 Circuit & Wiring](#-circuit--wiring)
- [💻 Software & Libraries](#-software--libraries)
- [🚀 Quick Start](#-quick-start)
- [📅 Progress Log](#-progress-log)
- [🗺️ Roadmap](#️-roadmap)
- [❓ Troubleshooting & FAQ](#-troubleshooting--faq)
- [📚 References & Inspiration](#-references--inspiration)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

---

## 📖 About

This repository documents the **end-to-end journey** of building a **Smart Plant Watering System** using the **ESP32** microcontroller — from idea → research → prototyping → IoT cloud integration.

Every decision, component, wiring change, and day's work is tracked in Markdown so the project is **reproducible, reviewable, and portfolio-ready**.

**Why ESP32?**

| Reason | Detail |
|--------|--------|
| Wi-Fi + Bluetooth built-in | No extra shield needed for IoT |
| Dual-core, low power | Can deep-sleep between readings |
| Plenty of ADC pins | `GPIO34` is ADC1 → safe with Wi-Fi (ADC2 conflicts) |
| Large community | Arduino IDE & Blynk have first-class ESP32 support |
| Cheap & available | ESP32 DevKit V1 ~ $5–8 |

> **Build variant in this repo:** `ESP32 DevKit V1` + `Capacitive Soil Moisture Sensor` + `Relay + 5V Submersible Pump` + `Blynk IoT` for remote monitoring/control.

---

## 🌿 What is an Automatic Plant Watering System?

An automatic plant watering system is a **closed-loop irrigation system** that:

1. **Senses** — continuously measures soil moisture (and optionally temperature/humidity)
2. **Decides** — compares the reading to a threshold (e.g., `<30%` = dry)
3. **Acts** — switches a water pump/valve **ON/OFF** via a relay/MOSFET
4. **Reports** — optionally sends data to a phone/cloud dashboard

**Why it matters:**

- 💧 **Saves water** — waters only when needed (studies report 30–50% saving vs. timer-only)
- 🌱 **Healthier plants** — prevents over/under-watering, the #1 killer of houseplants
- 🏠 **Convenience** — perfect for travel, offices, and urban gardening
- 📊 **Data-driven** — learn your plant’s actual needs instead of guessing

**Three videos that inspired this build:**

| # | Video | Channel | What I Learned |
|---|-------|---------|----------------|
| 1 | [DIY Automatic Plant Watering System Using Arduino](https://www.youtube.com/watch?v=9x3YIiROZ3s) — Simple Circuits | Arduino + soil sensor + relay + pump. Core idea: `dry → pump ON`, `wet → pump OFF`. Critical lesson: **never power the pump from the MCU** — use separate battery/supply. | Baseline circuit + pump isolation |
| 2 | [How to make a plant watering system with ESP32 board and Blynk app](https://www.youtube.com/watch?v=Ix3a3ThsHfA) — SriTu Hobby | ESP32 + capacitive sensor + LCD (I2C) + relay + Blynk. Shows cloud monitoring, local display, and threshold tuning. | ESP32 + IoT (Blynk) architecture I will follow |
| 3 | [How to make a plant watering system with the Nodemcu ESP8266 board and the new Blynk update](https://www.youtube.com/watch?v=WLCR0r7rYi8) — SriTu Hobby | NodeMCU ESP8266 + soil sensor + relay + LCD + new Blynk 2.0 (V0 moisture, V1 pump). Includes blink test, LCD check, Arduino IDE, breadboard, Blynk relay→pump switch. | ESP8266 fallback + Blynk 2.0 workflow + **Day 02 working bench test** |

> 📝 Full notes on all videos are in [`docs/references.md`](docs/references.md) and [`docs/videos/02-nodemcu-esp8266-blynk-update.md`](docs/videos/02-nodemcu-esp8266-blynk-update.md).

---

## 🌐 What is IoT?

**IoT — Internet of Things** is a network of physical objects (“things”) embedded with **sensors, software, and connectivity** that collect and exchange data over the internet without constant human intervention. Term coined by **Kevin Ashton (1999)** at MIT Auto-ID Lab.

**In this project:**

```
[Soil Sensor] → [ESP32] → [Wi-Fi] → [Blynk Cloud] → [Your Phone]
     ↑              ↓
  (senses)     (decides + acts → Relay → Pump)
```

- **Without IoT:** The ESP32 waters locally but you have no visibility when away.
- **With IoT:** You see moisture %, get “Pump ON” alerts, override manually from anywhere, and log history to Google Sheets.

**Key IoT concepts used here:**

| Concept | In this project |
|---------|-----------------|
| **Sensing** | Capacitive soil moisture → analog voltage → % |
| **Edge computing** | ESP32 thresholds + debouncing + burst logic |
| **Connectivity** | ESP32 Wi-Fi → Blynk MQTT/HTTP |
| **Cloud dashboard** | Blynk App: gauges, buttons, notifications |
| **Remote actuation** | Manual pump ON/OFF from phone |
| **Data logging** | Blynk history / optional Google Sheets webhook |

> Learn more: [`docs/overview.md`](docs/overview.md) — deep dive into IoT + smart irrigation.

---

## ⚙️ How It Works — System Architecture

### Block Diagram

```
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│  Capacitive  │─────▶│              │─────▶│   1-Ch Relay │
│ Soil Moisture│ AOUT │   ESP32      │ GPIO │   Module     │
│   Sensor     │      │  DevKit V1   │  26  │  (Active LOW)│
└──────────────┘      │              │      └──────┬───────┘
                      │  ┌────────┐  │               │
┌──────────────┐      │  │ Wi-Fi  │  │               ▼
│  (Optional)  │─────▶│  └───┬────┘  │      ┌────────────────┐
│ DHT11/22 or  │ GPIO │      │       │      │ 5V Submersible │
│ OLED 0.96"   │      │      ▼       │      │  Water Pump    │
└──────────────┘      └──┬──────┬────┘      │  + Tubing +    │
                         │      │           │  Reservoir     │
                    Blynk Cloud  └──────────│  (separate PSU)│
                    (V0..V5)                └────────────────┘
```

### Control Flow (Auto Mode)

```
loop every 2–5 sec
  │
  ├─▶ read soilMoisture (ADC → % with calibration)
  │
  ├─▶ if moisture < DRY_THRESHOLD (e.g., 30%) → Relay LOW → Pump ON (burst 5–10s)
  │                                              cap max ON-time to avoid flooding
  │
  ├─▶ if moisture > WET_THRESHOLD (e.g., 60%)  → Relay HIGH → Pump OFF (hysteresis)
  │
  ├─▶ update Blynk: V2 = moisture %, V3 = pump state
  │
  └─▶ (Manual override) if Blynk V5 == 1 → force pump ON regardless
```

**Why hysteresis?** Soil wetting is slow. Without a gap (30% ON / 60% OFF) the pump would chatter ON/OFF every second.

**Safety caps:** Max 10 s burst, min 30 s cooldown, reservoir-low interlock (if added later).

Detailed logic: [`docs/circuit.md`](docs/circuit.md) and [`docs/firmware-setup.md`](docs/firmware-setup.md).

---

## ✨ Features

| Feature | Status | Notes |
|---------|--------|-------|
| Bench prototype (ESP + sensor + relay + pump + LCD on breadboard) | ✅ **Complete** | See [Day 03 photos](progress/day-03.md) — LCD `59%` proof |
| Capacitive sensing (corrosion-proof) | ✅ Chosen | Not resistive forks — sensor in cup test `59%` |
| Blynk live dashboard | ✅ **Working** | Web + App: V0 moisture gauge, V1 pump button — tested Day 02, now on bench Day 03 |
| Manual override (Blynk relay switch → pump) | ✅ **Working** | `BLYNK_WRITE(V1)` → relay LOW/HIGH → pump ON/OFF verified |
| Arduino IDE + ESP board bring-up | ✅ **Done** | Board package + driver + Blink test (GPIO2/D4) pass |
| Breadboard & small tools | ✅ **Done** | Full assembly Day 03: rails, M-M/M-F jumpers, 18650 isolate — 📸 [overview](docs/images/day-03-bench-overview.jpg) |
| LCD local display (16×2 + I2C `0x27`) | ✅ **Live 59%** | `Moistire :59` on LCD — 📸 [close-up](docs/images/day-03-lcd-59-detail.jpg) |
| Auto watering on dry soil (threshold + hysteresis) | 🔜 Next | Optional: `<30% ON / >60% OFF` |
| Low-water / reservoir alert | 💡 Optional | Float switch on GPIO27 |
| Data logging to cloud | 💡 Optional | Blynk history / Google Sheets |
| Deep-sleep power saving | 💡 Future | For battery deployment |

Legend: ✅ Done · 🔜 Next · 💡 Optional/Stretch

---

## 🧰 Hardware — Bill of Materials

> Full sheet with **purpose + wiring + cost + alternatives**: [`docs/bill-of-materials.md`](docs/bill-of-materials.md)

**Core Kit (Minimum Viable Build):**

| # | Component | Qty | Purpose (one-line) | Est. Cost |
|---|-----------|-----|---------------------|-----------|
| 1 | **ESP32 DevKit V1 (30-pin)** | 1 | Brain + Wi-Fi; reads sensor, drives relay, talks to Blynk | $6–8 |
| 2 | **Capacitive Soil Moisture Sensor v1.2 / v2.0** | 1 | Measures soil moisture (analog voltage ∝ wetness) | $4–6 |
| 3 | **5V 1-Channel Relay Module (active-LOW)** | 1 | Electrically isolates ESP32 from pump current | $1.5–3 |
| 4 | **5V Submersible Mini Water Pump + Vinyl Tubing** | 1 | Delivers water from reservoir to pot | $4–7 |
| 5 | **5V/12V Pump Power Supply (separate)** | 1 | Pump draws 0.5–1A stall; never from ESP32 3.3V | $5–8 |
| 6 | **Breadboard + Jumper Wires (M-M, M-F)** | 1 set | Prototyping without soldering | $3–5 |
| 7 | **Micro-USB Cable (data)** | 1 | Flash ESP32 + serial monitor | $2 |
| 8 | **Plastic Reservoir + Plant Pot** | 1 | Water source & test plant | $1–3 |

**Nice-to-have / Day-1 gathered additions:**

| # | Component | Purpose |
|---|-----------|---------|
| 9 | 16×2 LCD + I2C Backpack *or* 0.96" OLED SSD1306 | Local moisture display |
| 10 | DHT11 / DHT22 | Air temp & humidity → smarter watering |
| 11 | Push Button (tactile) | Manual watering trigger |
| 12 | 18650 Li-ion + Holder + TP4056 | Portable power (SriTu Hobby variant) |
| 13 | Flyback Diode 1N4007 | Protect relay/pump from inductive spike |
| 14 | Waterproof Box (IP65) | Final enclosure |

---

## 🔧 Tools Required

> Full list with *why each is needed*: [`docs/tools.md`](docs/tools.md)

- **Software:** Arduino IDE 2.x (or VS Code + PlatformIO), Blynk IoT App (iOS/Android), Blynk Web Console, CP210x/CH340 USB driver
- **Hardware tools:** Screwdriver, wire stripper, multimeter, electrical tape / heat-shrink, optional soldering iron
- **Calibration helpers:** Glass of water + dry soil sample (for wet/dry ADC calibration)

---

## 🔌 Circuit & Wiring

> Detailed diagram + safety notes: [`docs/circuit.md`](docs/circuit.md)

**Core wiring (ESP32 DevKit V1):**

| From | To | Pin | Notes |
|------|----|-----|-------|
| Soil Sensor **VCC** | ESP32 **3V3** | — | Capacitive sensor runs at 3.3V (5V tolerant but 3.3V = safer ADC) |
| Soil Sensor **GND** | ESP32 **GND** | — | Common ground |
| Soil Sensor **AOUT** | ESP32 **GPIO34** | ADC1_CH6 | **Use ADC1 pin** (ADC2 conflicts with Wi-Fi). GPIO34 is input-only, perfect |
| Relay **VCC** | ESP32 **VIN (5V)** | — | Relay coil needs 5V; ESP32 VIN when USB-powered gives ~5V |
| Relay **GND** | ESP32 **GND** | — | Common ground |
| Relay **IN** | ESP32 **GPIO26** | D26 | Logic control. Code assumes **active-LOW** → `LOW = Pump ON`. Invert if yours is active-HIGH |
| Pump **+** (via Relay **NO**) | External PSU **+** | — | Relay **COM → PSU +**, **NO → Pump +** (Normally Open = safe default OFF) |
| Pump **−** | External PSU **−** | — | PSU GND → ESP32 GND (**common ground mandatory**) |

⚠️ **Safety rules:**

1. **Never power pump from ESP32 3.3V/5V pin** — back-EMF & stall current will brown-out or burn the board.
2. **Common GND** between ESP32 and pump PSU, otherwise relay switching is unreliable.
3. Add **flyback diode (1N4007)** across pump terminals (cathode to +) if not on relay board.
4. Keep sensor & pump wires **away** from each other to reduce noise on ADC.

---

## 💻 Software & Libraries

| Need | Choice | Install |
|------|--------|---------|
| IDE | Arduino IDE 2.3+ or PlatformIO | [arduino.cc/downloads](https://www.arduino.cc/en/software) |
| ESP32 core | `esp32 by Espressif Systems 3.x` | Boards Manager → search `esp32` |
| IoT cloud | **Blynk IoT** (free tier) | App Store + [blynk.cloud](https://blynk.cloud) |
| Libraries | `Blynk` (by Volodymyr Shymanskyy), `DHT sensor library` (if using DHT), `Adafruit Unified Sensor`, `SSD1306`/`LiquidCrystal_I2C` (if display) | Library Manager |
| USB driver | CP210x or CH340 (check your ESP32) | Manufacturer site |

See [`docs/firmware-setup.md`](docs/firmware-setup.md) for step-by-step flashing + Blynk virtual pins (`V0–V6`) + calibration sketch.

---

## 🚀 Quick Start

### 1. Clone & open

```bash
git clone https://github.com/Dilujan-Uki/Plant-watering-system.git
cd Plant-watering-system
# Open firmware/ in Arduino IDE or PlatformIO
```

### 2. Wire the core circuit (5 minutes)

Follow [`docs/circuit.md`](docs/circuit.md) — sensor to GPIO34, relay to GPIO26, separate pump supply with common GND.

### 3. Create Blynk project

1. Create account at [blynk.cloud](https://blynk.cloud) → New Template → Hardware: ESP32, Connection: WiFi
2. Copy `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`
3. In app: Add Datastreams — `V0 Temp`, `V1 Humidity`, `V2 Soil % (Gauge)`, `V3 Pump LED`, `V4 Auto/Manual Switch`, `V5 Manual Pump Button`

### 4. Flash calibration sketch (find your thresholds)

```cpp
// firmware/calibration/calibration.ino
// Upload, open Serial Monitor @115200, note values:
int dry = 3800; // sensor in air
int wet = 1600; // sensor in water
// Your threshold sits between: e.g., 30% dry → ~ 3100
```

### 5. Flash main firmware

Paste tokens into `firmware/plant-watering-esp32/plant-watering-esp32.ino` → Select Board: `ESP32 Dev Module` → Port → Upload → Open Serial Monitor.

### 6. Test

- Dry soil → pump bursts → Blynk notification
- Wet soil → pump off
- Blynk manual button → force pump

---

## 📅 Progress Log

> Every day’s work is logged as a standalone markdown. Start here:

| Day | Date | Focus | Log |
|-----|------|-------|-----|
| **Day 01** | 2026-10-26 | Research + Inspiration + Materials Gathering (ESP32 variant decision) | [`progress/day-01.md`](progress/day-01.md) |
| **Day 02** | **2026-10-29** | **Pump Motor & Relay Found · ESP Blink Test · LCD Check · Arduino IDE + Breadboard · Blynk Relay→Pump (WORKING)** | [`progress/day-02.md`](progress/day-02.md) · [video deep-dive](docs/videos/02-nodemcu-esp8266-blynk-update.md) |
| **Day 03** | **2026-10-30** | **Bench Prototype COMPLETE ✅ — LCD `Moistire :59` + ESP + Sensor + Relay + Pump + Blynk Switch (Photos)** | [`progress/day-03.md`](progress/day-03.md) with [📸 Bench](docs/images/day-03-bench-overview.jpg) · [📸 Reference](docs/images/day-03-blynk-reference.jpg) · [📸 LCD 59%](docs/images/day-03-lcd-59-detail.jpg) |
| Day 04 | — | Auto threshold + hysteresis + soil calibration (optional) | `TEMPLATE` |

📓 **Log index + how to write a daily log:** [`progress/README.md`](progress/README.md)
📄 **Daily template:** [`progress/TEMPLATE.md`](progress/TEMPLATE.md)

---

## 🗺️ Roadmap

- [x] **Day 01** — Study reference builds, define ESP32 route, gather BOM
- [x] **Day 02 (2026-10-29)** — Pump & relay found, ESP Blink ✅, LCD `0x27` check ✅, Arduino IDE + breadboard learned, Blynk IoT web+app relay→pump **WORKING** ✅
- [x] **Day 03 (2026-10-30)** — **Bench prototype COMPLETE ✅** — ESP + sensor + relay + pump + LCD + breadboard assembled, LCD shows **`Moistire :59`**, Blynk `V1` switch → pump verified, photos documented
- [ ] **Day 04** — Auto threshold + hysteresis + real-soil ADC calibration (dry/wet) + photos
- [ ] **Day 05** — DHT11/22 environment compensation (optional)
- [ ] **Day 06** — Reservoir low-water interlock + alerts
- [ ] **Day 07** — Enclosure + cable management + long-run test (24–48h)
- [ ] **Day 08** — Power optimization (deep-sleep) + documentation polish
- [ ] **Day 09** — Demo video + final README screenshots + release

Stretch: Multi-zone (2–4 plants), Google Sheets logging via Apps Script, MQTT/Home Assistant, solar power.

---

## ❓ Troubleshooting & FAQ

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Sensor reads `4095` always | Wrong pin (ADC2) or loose GND | Move to GPIO32–39 (ADC1). Check GND continuity |
| Pump never turns on | Relay logic inverted | Try `digitalWrite(RELAY, LOW)` = ON; swap HIGH/LOW |
| ESP32 resets when pump starts | Pump powered from ESP32 | Use separate 5V/2A supply + common GND |
| Blynk offline | Wrong `auth token` or Wi-Fi creds | Re-check `BLYNK_AUTH_TOKEN` + 2.4 GHz Wi-Fi (ESP32 not 5 GHz) |
| OLED garbage | Wrong I2C address | Scan with I2C scanner; try 0x3C vs 0x27 |
| Moisture % jumps | No averaging / noisy supply | Add 10-sample moving average + 100nF cap on sensor VCC |

**Calibration reminder:** Every sensor + soil + pot is different. Re-run dry/wet calibration when you change soil or sensor depth.

---

## 📚 References & Inspiration

Full annotated list: [`docs/references.md`](docs/references.md)

- 🎥 [DIY Automatic Plant Watering System Using Arduino](https://www.youtube.com/watch?v=9x3YIiROZ3s) — Simple Circuits (Arduino baseline build)
- 🎥 [How to make a plant watering system with ESP32 board and Blynk app](https://www.youtube.com/watch?v=Ix3a3ThsHfA) — SriTu Hobby (ESP32 + Blynk + LCD/relay — primary blueprint)
- 🎥 [How to make a plant watering system with the Nodemcu ESP8266 board and the new Blynk update](https://www.youtube.com/watch?v=WLCR0r7rYi8) — SriTu Hobby (ESP8266 + new Blynk 2.0 + Blink + LCD + breadboard + App relay→pump — **Day 02 working test**) — [deep dive](docs/videos/02-nodemcu-esp8266-blynk-update.md) + [article](https://srituhobby.com/how-to-make-a-plant-watering-system-with-the-nodemcu-esp8266-board-and-the-new-blynk-update/)
- 📖 SriTu Hobby Write-up: [srituhobby.com — Plant Watering System with ESP32 & Blynk](https://srituhobby.com/how-to-make-a-plant-watering-system-with-esp32-board-and-blynk-app/)
- 📖 Espressif — [ESP32 ADC & Wi-Fi notes (use ADC1 with Wi-Fi)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
- 📖 Blynk Docs — [ESP32 Quickstart](https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk) & Virtual Pins
- 📄 IRJMETS Paper — *IoT-Based Plant Watering System* (Nov 2024) — academic system overview
- 🔧 Community Example — [Chiara2804/Plant-Watering-System](https://github.com/Chiara2804/Plant-Watering-System) — EMA filtering, OLED emoji, Google Sheets logging

---

## 🤝 Contributing

This is a learning-in-public build. Contributions, suggestions, and field-test results are welcome!

1. Fork → feature branch → PR
2. For hardware changes, include a photo/diagram
3. Daily logs: copy [`progress/TEMPLATE.md`](progress/TEMPLATE.md)

---

## 📄 License

MIT — see [LICENSE](LICENSE). Free for personal, educational, and commercial use with attribution.

---

<p align="center"><sub>Built with 🌿 by <a href="https://github.com/Dilujan-Uki">Dilujan</a> · ESP32 + Blynk · Kandy, LK · 2026</sub></p>
