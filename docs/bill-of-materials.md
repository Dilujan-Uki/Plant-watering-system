# Bill of Materials (BOM) — Components & Purpose

> Every part in this build, why it exists, and what to buy instead if it’s out of stock.

**Last updated:** 2026-10-26 (Day 01 — Research & Gathering)
**Variant documented:** ESP32 DevKit V1 + Capacitive sensor + Single-channel relay + 5 V submersible pump + Blynk

---

## 1. Core Components (MVP — Must Have)

| # | Component | Qty | Model / Spec to Buy | Purpose — Why it’s needed | Notes / Alternative |
|---|-----------|-----|---------------------|---------------------------|---------------------|
| 1 | **ESP32 Development Board** | 1 | ESP32 DevKit V1, 30-pin (CP2102 or CH340 USB) | **Brain + Wi-Fi.** Reads analog sensor, runs threshold logic, drives relay, connects to Blynk cloud. Dual-core, deep-sleep capable. | Any DevKitC / NodeMCU-32S OK. Avoid bare ESP32-WROOM without dev board (needs USB + regulator). |
| 2 | **Capacitive Soil Moisture Sensor** | 1 | v1.2 or v2.0, 3-pin (VCC, GND, AOUT) | **Senses water content.** Outputs voltage inversely proportional to moisture — capacitive = corrosion-proof (no exposed electrodes). | ❌ Resistive fork sensors corrode in weeks. If stuck with resistive, coat & expect drift. |
| 3 | **Relay Module (1-Channel, 5 V)** | 1 | Active-LOW, opto-isolated (JD-VCC jumper optional) | **Isolation + switching.** Lets 3.3 V logic control 5–12 V pump current (0.5–1 A). Protects ESP32 from stall/back-EMF. | Alternative: logic-level N-MOSFET (IRLZ44N / AOD4184) + 1N4007 flyback diode — quieter, no click. |
| 4 | **Submersible Mini Water Pump** | 1 | DC 3–5 V, 100–300 L/H, with filter | **Actuator.** Moves water from reservoir to soil via tubing. | Peristaltic pump = more precise dosing, self-priming, but 3× cost. Solenoid valve = needs pressurized line. |
| 5 | **Vinyl / Silicone Tubing** | 0.5–1 m | 6 mm ID, food-grade if possible | **Water path** from pump to pot. | Keep run <1 m for small pumps; avoid kinks. |
| 6 | **Pump Power Supply (separate)** | 1 | 5 V 2 A DC adapter (or 18650×2 + holder) | **Pump power.** Pumps draw surge >500 mA — far beyond ESP32’s regulator. | If using 12 V pump, buy 12 V 1–2 A adapter. **Must share GND with ESP32.** |
| 7 | **Breadboard + Jumper Wires** | 1 set | 830-tie + M-M, M-F, F-F 20 cm | **Prototyping** without soldering; test wiring quickly. | For final build: solder to perfboard/PCB + waterproof box. |
| 8 | **Micro-USB Data Cable** | 1 | 480 Mbps data (not charge-only) | **Flashing & serial debug** via Arduino IDE. Many “charging” cables lack data pins. | Test: does device show in Device Manager / `ls /dev/tty*`? |
| 9 | **Water Reservoir + Pot + Soil** | 1 each | Any plastic container (1–5 L) | **System under test.** Reservoir holds water; pot is the controlled plant. | Transparent reservoir helps see level. |
| 10 | **Flyback Diode 1N4007** | 1 | 1 A / 1000 V axial | **Protects relay contacts & transistor** from inductive kick when pump switches off. Some relay boards already include it. | Skip only if relay module has built-in diode (most do). Keep one spare. |

---

## 2. Optional / Gathered on Day 01 (SriTu Hobby Variant + Enhancements)

| # | Component | Qty | Purpose | When to add |
|---|-----------|-----|---------|-------------|
| 11 | **16×2 LCD + I2C Backpack (PCF8574)** | 1 | Local display: `Moisture: 42% Pump: OFF` — debug without phone | Day 02–03, mirrors Blynk state |
| 12 | **OLED SSD1306 0.96" I2C (128×64)** | 1 alt to LCD | Sharper local UI; shows emoji/status; lighter wiring (SDA/SCL only) | Preferred over 16×2: less pins, modern look |
| 13 | **DHT11 / DHT22 (AM2302)** | 1 | Air temp & humidity; adjust watering by evaporation | After MVP works; DHT22 more accurate than DHT11 |
| 14 | **Tactile Push Button (6×6 mm)** | 1–2 | Manual watering trigger / mode toggle without phone | Day 03 — wire with pull-up to GND |
| 15 | **18650 Li-ion Battery ×2 + 2-slot Holder + TP4056 charger** | 1 set | Portable power for demo / no mains nearby | SriTu Hobby variant; add fusing |
| 16 | **DS3231 RTC Module** | 1 | Accurate time scheduling (e.g., “don’t water at noon”) if no Wi-Fi | Only if need offline schedules without NTP |
| 17 | **Water Level / Float Switch** | 1 | Reservoir-low detection → interlock pump + “Refill!” alert | Prevent pump dry-run |
| 18 | **Status LEDs (Red/Green/Yellow) + 220 Ω resistors** | 3 | At-a-glance: dry / OK / watering (good for photos/demos) | Low-cost visual feedback |
| 19 | **Buzzer (5 V active)** | 1 | Audible dry-soil alarm | If no phone nearby |
| 20 | **Waterproof Enclosure (IP65) + Cable Glands** | 1 | Protect electronics from splashes/soil | Final enclosure stage |
| 21 | **Perfboard / PCB + Header Pins + Screw Terminals** | 1 | Permanent wiring after breadboard proven | Before deployment |

---

## 3. Consumables & Spares

| Item | Why |
|------|-----|
| Electrical tape + heat-shrink + zip ties | Insulate pump joints near water |
| Small cable ties / adhesive mounts | Route tubing along pot edge |
| Silica gel packet | Inside enclosure to fight condensation |
| Spare jumper wires | They break; keep 10 extra |
| Distilled water | For wet-calibration baseline |

---

## 4. Cost Estimate (Sri Lanka / Amazon — Oct 2026 ballpark)

| Kit | Includes | Estimate |
|-----|----------|----------|
| **MVP (core 1–9)** | ESP32 + capacitive sensor + relay + pump + PSU + breadboard + cable | **$25–40 USD** / LKR 7,500–12,000 |
| **Full (with display + DHT + battery)** | MVP + OLED/LCD + DHT22 + 18650 + buzzer | **$40–55 USD** |
| Per-plant extra | +1 sensor + relay channel + pump + tubing | +$10–15 per zone |

> Prices vary; local markets (Colpetty, Unity Plaza, Kandy tech shops) often cheaper than Amazon for ESP32 kits.

---

## 5. How to Choose Substitutions

| If you can’t find… | Buy instead | Trade-off |
|---------------------|-------------|-----------|
| ESP32 DevKit V1 | ESP32-WROOM DevKitC / NodeMCU-32S / ESP32-S3 DevKit | Pin labels shift — remap GPIO34/26 in code |
| Capacitive v1.2 | Capacitive v2.0 (taller, voltage regulator on board) | Same AOUT; v2.0 more robust |
| 5 V relay (active-LOW) | Logic-level MOSFET module (e.g., IRF520 — but needs true logic-level; IRLZ44N better) | Silent; needs flyback diode |
| 5 V pump | 12 V pump + 12 V adapter + relay rated 10 A | More pressure; heavier PSU |
| OLED | 16×2 LCD+I2C | OLED needs 2 wires (I2C) vs LCD 2 wires too — both OK |

---

## 6. Supplier Links (reference, not affiliate)

- SriTu Hobby store list (ESP32 adapter, sensor, LCD+I2C, relay, breadboard, 18650 holder) — see [references.md](references.md)
- Amazon alternatives — same links in SriTu Hobby description (ESP32 board, pump+pipe, Li-ion, holder)
- Local LK: search “ESP32 DevKit” + “capacitive soil moisture” on Daraz.lk / MDZone LK

---

## Next

- **Tools you need:** [`tools.md`](tools.md)
- **Wiring guide:** [`circuit.md`](circuit.md)
- **Progress — what was gathered Day 01:** [`../progress/day-01.md`](../progress/day-01.md)
