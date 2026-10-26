# Tools Required — Hardware & Software

> Everything you need to build, flash, and debug — split into software (on your PC/phone) and hardware (on your desk).

---

## 1. Software Tools

| # | Tool | Version / Source | Purpose | Install Check |
|---|------|------------------|---------|---------------|
| 1 | **Arduino IDE** | 2.3.x + (recommended) — or **PlatformIO** in VS Code | Write, compile, and upload firmware to ESP32 | Open IDE → Boards Manager should list `esp32` |
| 2 | **ESP32 Board Package** | `esp32 by Espressif Systems` 3.x | Adds ESP32 compiler + pin definitions to Arduino IDE | Boards Manager → search `esp32` → Install |
| 3 | **Blynk IoT App + Web Console** | iOS/Android app + https://blynk.cloud | Remote dashboard: gauges, buttons, notifications, history | Create free account → New Template → ESP32/WiFi |
| 4 | **CP210x / CH340 USB Driver** | Depends on ESP32 variant (check silkscreen near USB) | Lets PC recognize ESP32 serial port | Device Manager → Ports (COM & LPT) shows `Silicon Labs CP210x` or `CH340` |
| 5 | **Serial Monitor** | Built into Arduino IDE (115200 baud) | View `Serial.println()` logs: moisture %, relay state, Wi-Fi status | Upload → Tools → Serial Monitor → 115200 |
| 6 | **Git + GitHub** | Latest | Version control & daily progress logs (this repo) | `git --version` + GitHub account |
| 7 | **Markdown Editor** | VS Code + Markdown Preview (or HackMD) | Edit `README.md` + `progress/day-*.md` with live preview | VS Code → `Ctrl+Shift+V` preview |
| 8 | **Fritzing / draw.io / Excalidraw** | Optional | Draw circuit/wiring diagrams for docs | Export PNG → embed in `docs/circuit.md` |

### 1.1 Arduino Libraries (installed via Library Manager)

| Library | Author | For | Include |
|---------|--------|-----|---------|
| `Blynk` | Volodymyr Shymanskyy | Blynk cloud comms | `#include <BlynkSimpleEsp32.h>` |
| `DHT sensor library` | Adafruit | DHT11/22 temp/humidity | `#include "DHT.h"` (only if using DHT) |
| `Adafruit Unified Sensor` | Adafruit | Dependency of DHT library | — |
| `Adafruit SSD1306` + `Adafruit GFX` | Adafruit | OLED 0.96" display | Only if OLED added |
| `LiquidCrystal_I2C` | Frank de Brabander | 16×2 LCD via I2C backpack | Only if LCD added |

> Install: Arduino IDE → Sketch → Include Library → Manage Libraries → search by name → Install.

### 1.2 Blynk Virtual Pins (convention used in this repo)

| Virtual Pin | Widget | Purpose |
|-------------|--------|---------|
| **V0** | Value Display | Air temperature (°C) |
| **V1** | Value Display | Air humidity (%) |
| **V2** | Gauge / Value | Soil moisture **%** (0–100) — main metric |
| **V3** | LED | Pump state (1 = ON, 0 = OFF) |
| **V4** | Switch | Mode: 0 = Auto, 1 = Manual |
| **V5** | Button (push) | Manual pump ON (while held / toggle) |
| **V6** | Notification | Low water / dry alert |

You can rename/reassign — keep this table in sync if you change pins.

---

## 2. Hardware Tools

| # | Tool | Purpose | Alternative |
|---|------|---------|-------------|
| 1 | **Screwdriver (small Phillips)** | Tighten relay screw terminals, enclosure screws | Any small cross-head |
| 2 | **Wire stripper / flush cutter** | Strip jumper wires, trim tubing — clean joints | Nail scissors in a pinch |
| 3 | **Multimeter** | Check continuity (GND), measure 5 V rail, verify relay click | Essential for “is it getting power?” debugging |
| 4 | **Electrical tape / heat-shrink tubing** | Insulate pump-→-relay joints near water | Hot glue + tape if no heat gun |
| 5 | **Small pliers / tweezers** | Hold tiny I2C header pins, route tubing | — |
| 6 | **USB Power Meter (optional, $5)** | See live mA draw — confirm pump surge vs idle | Or just watch for ESP32 resets (brown-out sign) |
| 7 | **Soldering iron + solder (optional for now)** | Permanent build on perfboard later | Not needed for breadboard phase |

---

## 3. Calibration Helpers (no purchase needed)

| Item | Use |
|------|-----|
| Glass of water | **Wet reference** — dip sensor → record ADC (e.g., ~1600) |
| Dry soil in pot / sensor in air | **Dry reference** — record max ADC (e.g., ~3800) |
| Phone stopwatch | Time pump burst → measure mL per 5 s → tune max ON time |
| Small measuring cup (100 mL) | Quantify watering volume |

---

## 4. What You Need on Day 01 vs. Later

| Phase | Tools truly needed |
|-------|---------------------|
| **Day 01 (Research & Gathering)** | Browser, Markdown editor, Git — no hardware wiring yet ✅ |
| **Day 02** | Multimeter, USB cable, Arduino IDE — first ADC test |
| **Day 03** | Blynk app + Wi-Fi — cloud dashboard test |
| **Day 04+** | Pump + external PSU + reservoir — never on Day 01 |

> Don’t connect the pump on the first wiring day — validate sensor readings first.

---

## 5. Troubleshooting Tool Tips

| Problem | Tool that helps |
|---------|-----------------|
| “Port not found” | CP210x/CH340 driver + try different USB cable |
| Relay clicks but pump silent | Multimeter on relay NO→COM; check PSU voltage under load |
| Moisture % stuck at 0/100 | Serial Monitor → raw ADC; re-run dry/wet calibration |
| Intermittent Wi-Fi | Phone hotspot test; check 2.4 GHz (ESP32 lacks 5 GHz) |

More: [`circuit.md`](circuit.md) Safety Notes & [`firmware-setup.md`](firmware-setup.md) Calibration.

---

## Next

- **Wiring:** [`circuit.md`](circuit.md)
- **Code:** [`firmware-setup.md`](firmware-setup.md)
- **Log Day 01:** [`../progress/day-01.md`](../progress/day-01.md)
