# Circuit & Wiring Guide — ESP32 + Soil Sensor + Relay + Pump

> The only wiring you need for the MVP. Copy this table and you’ll have a working rig in minutes.

---

## 1. Wiring Overview

```
                USB 5V (dev)
                     │
          ┌──────────▼──────────┐
          │      ESP32 DevKit V1│
  Soil ──▶│ GPIO34 (ADC1)  GPIO26│──▶ Relay IN
  Sensor  │ 3V3   GND    VIN  GND│    (active-LOW)
          └────┬───┬──────┬───┬──┘
               │   │      │   │
               │   │      │   └───── GND ─────┐
               │   │      │                   │
               │   │      └─ 5V (VIN) ──▶ Relay VCC
               │   │                          │
               │   └────── GND ──▶ Relay GND & Sensor GND
               │                              │
               └────── 3V3 ──▶ Sensor VCC     │
                                              │
         External 5V/12V PSU ──COM──Relay──NO──▶ Pump + (switched)
                   │                  │         Pump − ──▶ PSU ─
                   └──── GND ─────────┴──── ESP32 GND  (COMMON GND!)
```

---

## 2. Pin-by-Pin Table (Core Build)

| # | From | To | ESP32 Pin | Wire Color Hint | Notes |
|---|------|----|-----------|-----------------|-------|
| 1 | Soil Sensor **VCC** | ESP32 **3V3** | 3V3 | Red | Capacitive sensor works at 3.3–5.5 V; 3.3 V = safest ADC range |
| 2 | Soil Sensor **GND** | ESP32 **GND** | GND | Black | Keep short; star-ground near ESP32 |
| 3 | Soil Sensor **AOUT** | ESP32 **GPIO34** | ADC1_CH6 | Yellow/Green | **Must be ADC1 pin (32–39)** — ADC2 unusable with Wi-Fi. GPIO34 is input-only = ideal |
| 4 | Relay **VCC** | ESP32 **VIN (5V)** | VIN | Red | Relay coil draws ~70 mA; needs 5 V. Only present when USB plugged |
| 5 | Relay **GND** | ESP32 **GND** | GND | Black | Common ground |
| 6 | Relay **IN** | ESP32 **GPIO26** | GPIO26 | Blue/White | Control line. This repo = active-LOW (LOW→pump ON). Invert if module is active-HIGH |
| 7 | Relay **COM** | Pump PSU **+** | — | Red | Pump supply common |
| 8 | Relay **NO** | Pump **+** wire | — | Red | Normally-Open = pump OFF by default (safe) |
| 9 | Pump **−** | Pump PSU **−** (GND) | — | Black | Direct return |
| 10 | Pump PSU **GND** | ESP32 **GND** | GND | Black | **Mandatory common ground** — relay won’t switch reliably without it |

> **Don’t use NC** (Normally Closed) — it would run the pump when ESP32 is off/rebooting.

---

## 3. Optional Add-ons Wiring

### 3.1 OLED SSD1306 0.96" (I2C)

| OLED Pin | ESP32 Pin |
|----------|-----------|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |
| I2C Address | 0x3C (scan if unsure) |

### 3.2 16×2 LCD + I2C Backpack

| LCD I2C Pin | ESP32 Pin |
|-------------|-----------|
| VCC | 5V (VIN) |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |
| Address | Often 0x27 or 0x3F — run I2C scanner sketch |

### 3.3 DHT11 / DHT22

| DHT Pin | ESP32 Pin | Extra |
|---------|-----------|-------|
| VCC | 3V3 | — |
| DATA | GPIO4 (or GPIO14) | 10 kΩ pull-up to 3V3 (some modules have it built-in) |
| GND | GND | — |

### 3.4 Manual Push Button

| Button Pin | ESP32 Pin |
|------------|-----------|
| One leg | GPIO13 (or GPIO32) |
| Other leg | GND |
| Mode | `pinMode(BTN, INPUT_PULLUP)` → pressed = `LOW` |

### 3.5 Reservoir Float / Water Level Sensor

| Sensor Pin | ESP32 Pin |
|------------|-----------|
| AO / OUT | GPIO35 (ADC1) or GPIO27 digital (LOW = low water) |
| VCC | 3V3 |
| GND | GND |

---

## 4. Calibration & Logic Snapshot

```cpp
// Pseudocode — full sketch in firmware/
const int SOIL_PIN  = 34;   // ADC1 only!
const int RELAY_PIN = 26;   // active-LOW: LOW = pump ON
const int DRY_ADC   = 3800; // calibrate: sensor in air
const int WET_ADC   = 1600; // calibrate: sensor in water

int moisturePct = map(analogRead(SOIL_PIN), DRY_ADC, WET_ADC, 0, 100);
moisturePct = constrain(moisturePct, 0, 100);

// Hysteresis (avoid pump chatter)
if (moisturePct < 30) digitalWrite(RELAY_PIN, LOW);   // ON  — dry
if (moisturePct > 60) digitalWrite(RELAY_PIN, HIGH);  // OFF — wet

// Safety
// - max burst 10 s, then forced OFF + 30 s cooldown
// - if reservoir LOW pin == HIGH → never turn ON
```

**Find your thresholds:**

1. Upload a sketch that prints `analogRead(GPIO34)` every 500 ms.
2. Hold sensor in air → note `DRY_ADC`.
3. Dip sensor in water (not submerging electronics) → note `WET_ADC`.
4. Bury in *your* soil, water fully, let drain → your personal wet/dry window.

---

## 5. Safety & Reliability Checklist

- [ ] **Separate PSU for pump** — ESP32 USB/Battery does *not* drive pump current
- [ ] **Common GND** — ESP32 GND tied to pump PSU GND
- [ ] **Flyback diode** — 1N4007 across pump (cathode to +), if not on relay board
- [ ] **Tubing secured** — zip-tie so vibration doesn’t dislodge it into electronics
- [ ] **No mains** involved — only 5 V / 12 V DC here; keep AC adapters away from water
- [ ] **Relay COM→NO (not NC)** — safe state = OFF
- [ ] **Drip loop** — tubing loops below electronics so leaks drip down, not into enclosure
- [ ] **Fuse option** — 2 A inline fuse on pump + lead for final build

### Common Pitfall: ADC2 with Wi-Fi

> ESP32 has ADC1 (GPIO32–39) and ADC2 (GPIO0,2,4,12–15,25–27). **ADC2 is internally used by Wi-Fi** — analog reads glitch when Wi-Fi is active. Always use **ADC1 pins** for sensors when using Blynk/Wi-Fi. That’s why we use **GPIO34**.

### Common Pitfall: Power Brown-out

Symptom: ESP32 resets the moment pump starts.
Cause: Pump surge steals 5 V rail.
Fix: Separate PSU, add 100–470 µF cap across relay VCC/GND, shorten wires, use 2 A+ PSU.

---

## 6. Photo / Diagram Placeholder

> Add your real photos after wiring:
>
> - `docs/images/wiring-breadboard.jpg` — top-down breadboard shot (label GPIO34 + GPIO26 with tape)
> - `docs/images/pump-relay-wiring.jpg` — relay to pump joints (show insulated tape/heat-shrink)
> - `docs/images/circuit-fritzing.png` — Fritzing export (optional)
>
> Replace this placeholder box with:
>
> ```md
> ![Breadboard wiring](images/wiring-breadboard.jpg)
> ![Pump isolation](images/pump-relay-wiring.jpg)
> ```

---

## Next

- **Firmware & Blynk:** [`firmware-setup.md`](firmware-setup.md)
- **BOM:** [`bill-of-materials.md`](bill-of-materials.md)
- **Daily log:** [`../progress/day-01.md`](../progress/day-01.md)
