# Overview — What is a Plant Watering System & What is IoT?

> A beginner-friendly primer that explains the *why* behind this project before the *how*.

---

## 1. What is an Automatic Plant Watering System?

### 1.1 Definition

An **Automatic Plant Watering System** (also called *smart irrigation* or *auto-irrigation*) is an embedded system that:

1. **Senses** soil condition (usually soil moisture, optionally temperature, humidity, light)
2. **Makes a decision** based on a rule/threshold
3. **Acts** by opening a water path (pump, solenoid valve, peristaltic pump)
4. **(Optionally) Informs** a human via a display, app, or cloud log

Unlike a simple timer (“water 10 s every 12 h”), a sensor-driven system waters **only when the plant needs it**, which:

- saves water
- reduces root rot / drought stress
- works while you’re away

### 1.2 Everyday Analogy

Think of it like a **thermostat for water**:

- Thermostat: *too cold → heater ON, warm enough → heater OFF*
- This system: *soil too dry → pump ON, soil wet enough → pump OFF*

The “temperature” here is **soil moisture %**, measured by the sensor.

### 1.3 Types of Systems

| Type | How it decides | Pros | Cons |
|------|----------------|------|------|
| **Timer-based** | Fixed schedule | Simple | Wastes water; ignores rain/plant need |
| **Sensor-threshold (this project)** | If moisture < X% → water | Efficient, adaptive | Needs calibration |
| **Smart / ML-based** | Weather forecast + history + soil | Most efficient | Complex; needs cloud data |
| **Manual IoT** | User taps button remotely | Flexible | Not autonomous |

This repo starts with **sensor-threshold** and evolves toward **IoT-manual + auto hybrid** (Blynk auto + manual override).

### 1.4 Where it’s used

- Home: potted plants, balcony herbs, terrace gardens
- Education: Arduino/ESP32 coursework (very popular in YouTube/college demos)
- Agriculture: drip irrigation for rows/zones
- Research: greenhouse automation

---

## 2. What is IoT (Internet of Things)?

### 2.1 Simple Definition

> **IoT = Internet of Things** — a network of physical objects (“things”) fitted with sensors, processors, and network links that **collect, exchange, and act on data** over the internet, often without needing a human in the loop every time.

The term was coined in **1999 by Kevin Ashton** (MIT Auto-ID Lab, working on RFID at P&G). [Source: CropIn, IRJM ETS review papers]

At its core IoT has **four layers**:

```
[1. Perception]  →  [2. Network]  →  [3. Processing]  →  [4. Application]
   (sensors)         (Wi-Fi/MQTT)      (rules/cloud)      (app/dashboard)
```

### 2.2 IoT in this Project

| Layer | What we use |
|-------|-------------|
| **Perception** | Capacitive soil moisture sensor (analog), optional DHT11/22 |
| **Network** | ESP32 built-in Wi-Fi → Blynk Cloud (MQTT/HTTP underneath) |
| **Processing** | On-device: threshold + hysteresis + safety timers; Cloud: history, alerts |
| **Application** | Blynk mobile dashboard: gauge, pump status LED, manual button, notifications |

**Flow:**

```
Soil ──AOUT──▶ ESP32 ──Wi-Fi──▶ Blynk Cloud ──▶ Phone App
                │                    ▲
                └──▶ Relay ──▶ Pump ─┘ (actuation loop)
```

### 2.3 Why IoT vs. Plain Automation?

| Without IoT | With IoT |
|-------------|----------|
| Must be home to check/adjust | See moisture from anywhere |
| No history/logs | Graphs: “when did it last water?” |
| Fixed threshold in code | Change threshold from phone, no re-flash |
| Silent failures | “Reservoir low!” push notification |

You can build a working waterer without IoT, but IoT is what makes it a **smart system** and a **portfolio-grade IoT demo**.

### 2.4 Key IoT Concepts You’ll Touch

1. **Edge vs. Cloud** — ESP32 makes the fast “dry → water now” call at the edge; cloud handles slow “show last 7 days”.
2. **ADC & Calibration** — Raw `0–4095` → percentage; every soil/sensor is different.
3. **Actuation Isolation** — Low-voltage brain (ESP32) must not directly drive high-current loads (pump). Use relay/MOSFET + flyback diode.
4. **Hysteresis & Debouncing** — Avoid pump chatter; e.g., ON below 30%, OFF above 60%.
5. **Virtual Pins** — Blynk’s way to map data fields (V2 = soil %, V3 = pump state) like MQTT topics.
6. **Power Budget** — USB for dev, separate PSU for pump, deep-sleep for battery later.

---

## 3. How the Two Videos Connect

### Video 1 — Simple Circuits (Arduino baseline)

- **Link:** https://www.youtube.com/watch?v=9x3YIiROZ3s
- **Stack:** Arduino Uno + resistive moisture sensor + 5 V relay + DC pump + 9 V battery
- **Takeaways for this repo:**
  - Relay wiring: **COM → battery +**, **NO → pump +**, shared **GND**
  - Code: `if (analogRead(A0) < threshold) digitalWrite(RELAY, HIGH)`
  - Lesson: *separate supply for pump* — otherwise board resets/burns
- **Limitation we improve:** Arduino Uno has no Wi-Fi → we move to **ESP32** for IoT.

### Video 2 — SriTu Hobby (ESP32 + Blynk — blueprint)

- **Link:** https://www.youtube.com/watch?v=Ix3a3ThsHfA
- **Companion page:** https://srituhobby.com/how-to-make-a-plant-watering-system-with-esp32-board-and-blynk-app/
- **Stack:** ESP32 DevKit + capacitive sensor + 16×2 LCD (I2C) + relay + mini pump + **Blynk**
- **Takeaways:**
  - ESP32 pin choice matters: **GPIO34 (ADC1)** for sensor (ADC2 breaks with Wi-Fi)
  - Blynk datastreams for moisture, pump status, manual override
  - LCD locally mirrors cloud state (good for debugging without phone)
  - Power via 18650 + holder → portable demo

**This repo = Video 2 architecture + professional docs + daily logs + your ESP32 hardware.**

---

## 4. Glossary

| Term | Meaning |
|------|---------|
| **ADC** | Analog-to-Digital Converter; turns sensor voltage into 0–4095 number |
| **Capacitive sensor** | Measures dielectric constant of soil (no exposed metal → no corrosion), vs. resistive probes that rust |
| **Relay (active-LOW)** | Switch where `LOW` energizes the coil; `HIGH` = off. Common for cheap 5 V modules |
| **Flyback diode** | Diode across pump/valve to absorb voltage spike when motor turns off |
| **Hysteresis** | Two thresholds (e.g., 30% ON, 60% OFF) to stop rapid toggling |
| **Blynk** | IoT platform: drag-drop dashboards, manages Wi-Fi → phone bridge |
| **Virtual Pin (V0…)** | Blynk’s named data slots, like MQTT topics |
| **Deep-sleep** | ESP32 low-power mode (~10 µA); wakes to sample & water |

---

## Next

- **Components & purpose:** [`bill-of-materials.md`](bill-of-materials.md)
- **Tools you need:** [`tools.md`](tools.md)
- **Wiring & circuit:** [`circuit.md`](circuit.md)
- **Code & Blynk setup:** [`firmware-setup.md`](firmware-setup.md)
