# Day 01 — Research, Inspiration & Materials Gathering

**Date:** 2026-10-26 (Sunday)  
**Focus:** Understand the project, study reference builds, decide on ESP32 variant, gather materials  
**Status:** ✅ Completed  
**Time spent:** ~3–4 hours (research + sourcing)

---

## 🎯 Goal for Today

- [x] Understand what an **automatic plant watering system** is and why it’s useful
- [x] Understand what **IoT** means in the context of smart gardening/irrigation
- [x] Study 2 YouTube reference builds end-to-end
- [x] Decide on **hardware variant** — ESP32 (Wi-Fi/Blynk) over Arduino Uno
- [x] Gather / verify the core materials so building can start next session
- [x] Set up professional **Markdown documentation** structure for the whole project

---

## ✅ What I Did

### 1. Watched & Annotated Two Reference Videos

**Video 1 — Simple Circuits (Arduino baseline)**
- Link: https://www.youtube.com/watch?v=9x3YIiROZ3s
- Notes:
  - Understood the minimal circuit: soil moisture sensor → Arduino analog pin (A0) → logic → digital pin D7 → relay → pump
  - Relay wiring detail: `pump + → relay NO`, `battery + → relay COM`, `battery − + pump − → common GND`
  - Key lesson: **pump must NOT be powered from the Arduino’s 5V pin** — separate battery/adapter only, otherwise board burns/resets
  - Code pattern: `if (moisture < threshold) pump ON else OFF`
  - Good starting point, but no IoT / no remote monitoring

**Video 2 — SriTu Hobby (ESP32 + Blynk — primary blueprint)**
- Link: https://www.youtube.com/watch?v=Ix3a3ThsHfA
- Companion site: https://srituhobby.com/how-to-make-a-plant-watering-system-with-esp32-board-and-blynk-app/
- Notes:
  - Decided to **follow this architecture** for my build
  - Stack: **ESP32 DevKit → capacitive soil sensor → relay → mini pump → Blynk app**
  - Local feedback: **16×2 LCD + I2C module** (I may use OLED 0.96" SSD1306 instead — fewer pins)
  - Blynk: gauges for moisture, switch for pump, push notification on dry
  - Also uses **18650 Li-ion batteries + holder** for portability — good for demos without mains
  - Confirmed: capacitive sensor preferred over resistive (resistive corrodes)

### 2. Learned the Theory

- **What is a plant watering system?** — sensor-driven closed loop: sense soil → decide (threshold/hysteresis) → actuate pump → optionally report to cloud
- **What is IoT?** — Kevin Ashton 1999 definition: network of physical objects with sensors + connectivity → exchange data without constant human action
  - In this project: `Sensor → ESP32 (edge) → Wi-Fi → Blynk Cloud → Phone` + `Phone → Cloud → ESP32 → Relay → Pump` (remote override)
  - Without IoT = autonomous but blind; **with IoT = autonomous + visible + controllable**
- Browsed academic overviews (IRJMETS Nov 2024, E3S IoT irrigation papers) and community repos (Chiara2804, EricAbishekh24, surj10) for wiring/BOM patterns

### 3. Decided on ESP32 Variant

- **Choice: ESP32 DevKit V1 (30-pin) + Capacitive Soil Moisture Sensor v1.2/v2.0 + Single-channel 5 V Relay + 5 V Submersible Pump + Blynk**
- **Why ESP32 over Arduino Uno:**
  - Built-in Wi-Fi → no extra shield for IoT
  - More ADC pins + deep-sleep for future battery optimization
  - Large community + Blynk first-class support
  - Price parity (~$6–8) with far more capability
- Pinned rules: use **ADC1 pin GPIO34** for sensor (ADC2 conflicts with Wi-Fi), relay on **GPIO26**, common GND between ESP32 and pump PSU

### 4. Gathered Materials

| Component | Status |
|-----------|--------|
| ESP32 DevKit V1 board | ✅ Gathered |
| Capacitive soil moisture sensor | ✅ Gathered |
| 5 V 1-channel relay module | ✅ Gathered |
| Mini submersible water pump + vinyl tubing | ✅ Gathered |
| Breadboard + jumper wires (M-M, M-F) | ✅ Gathered |
| 5 V power adapter (separate for pump) | ✅ Gathered |
| Micro-USB data cable | ✅ Have |
| Reservoir container + test plant pot + soil | ✅ Have |
| (Optional) LCD 16×2 + I2C module / OLED 0.96" | 🔜 On hand — will wire after MVP |
| (Optional) DHT11/DHT22, push buttons, 18650 holder | 🔜 For later stages |

> Photos of gathered materials: to be added to `docs/images/day-01-materials.jpg` next edit

### 5. Created Professional Documentation

- Set up full repo docs structure:
  - `README.md` — comprehensive landing page (badges, TOC, what is IoT, how it works, BOM, wiring, quick start, roadmap, troubleshooting)
  - `docs/overview.md` — IoT & watering primer
  - `docs/bill-of-materials.md` — BOM with purpose/alternatives/cost
  - `docs/tools.md` — software + hardware tools + Blynk virtual pins
  - `docs/circuit.md` — pin-by-pin wiring + safety checklist
  - `docs/firmware-setup.md` — Arduino IDE + Blynk + calibration sketch + main firmware template
  - `docs/references.md` — annotated sources
  - `progress/README.md`, `progress/TEMPLATE.md`, `progress/day-01.md` (this file)
  - `firmware/calibration/calibration.ino` + `firmware/plant-watering-esp32/plant-watering-esp32.ino` + `firmware/plant-watering-esp32/secrets.h.example`
- All in **GitHub-flavored Markdown**, interlinked, ready for commits

---

## 🛠️ Materials / Tools Used Today

- **Materials handled:** ESP32 board, capacitive sensor, relay, pump, breadboard, tubing, reservoir (physical gathering + verification)
- **Tools used:**
  - Browser (YouTube, srituhobby.com, esp32io.com, GitHub)
  - Markdown editor (VS Code) + Git
  - GitHub repo: `Dilujan-Uki/Plant-watering-system`

---

## 🧩 Challenges & Fixes

| Challenge | What happened | How I handled it |
|-----------|---------------|------------------|
| Arduino vs ESP32 decision | Both videos use different boards; unclear which to pick | Compared features → ESP32 chosen for Wi-Fi/IoT; Arduino ideas kept as learning baseline |
| Conflicting pin advice online | Some guides use GPIO2/GPIO4 for sensor, others GPIO34 | Cross-checked with Espressif ADC docs: **ADC2 breaks with Wi-Fi → use ADC1 (GPIO32–39) → settled on GPIO34** |
| Many relay logic types | Some relays active-HIGH, some active-LOW | Documented both; code will handle with one invert (`LOW = ON` default) |
| Documentation scope | Wanted professional but not overwhelming | Followed GitHub README best-practice templates (badges → quick start → tables → links → progress) |

---

## 💡 Learning / Key Takeaways

1. **An auto-watering system is a sensor→decision→actuation loop** — IoT adds the *visibility + remote control* layer.
2. **Power isolation is non-negotiable** — microcontroller brain ≠ pump power; separate supply + common GND + flyback diode.
3. **Calibration is personal** — every soil/sensor/pot combo has its own dry/wet ADC window; no copy-paste threshold works without measuring.
4. **Good docs are a feature** — a well-structured README + daily logs makes the project credible and reproducible.

---

## ➡️ Next Steps (Day 02)

- [ ] Breadboard wiring: ESP32 + capacitive sensor only (no pump yet) → verify ADC on Serial Monitor
- [ ] Run `firmware/calibration/calibration.ino` → record dry/wet ADC values in table
- [ ] Take photos: gathered materials + first wiring + serial log screenshot → add to `docs/images/`
- [ ] Push Day 02 log and update `progress/README.md` index

---

## ⏱️ Time Spent

| Activity | Hours |
|----------|-------|
| Video study + note-taking | ~1.0 h |
| IoT + repo research (articles, GitHub examples) | ~1.0 h |
| Materials gathering / sourcing | ~0.5 h |
| Documentation setup (README + docs + progress) | ~1.5 h |
| **Total** | **~4.0 h** |

---

## 🖼️ Media

- 📸 *To add next:* `docs/images/day-01-materials.jpg` — flat-lay of ESP32 + sensor + relay + pump + tubing + breadboard
- 📸 *To add next:* `docs/images/day-01-research-notes.png` — screenshot of YouTube + srituhobby wiring page
- 📝 Serial logs: not yet — first logs come Day 02

---

## 🔗 Commits

- Branch: `arena/360e0168-plant-watering-system`
- Commit for this log: `docs: add day 01 — research, IoT learning & materials gathering`
- Related: `docs: overhaul README and add docs + progress + firmware templates`

---

*Template used: [`TEMPLATE.md`](TEMPLATE.md) · Index: [`README.md`](README.md) · Main README: [`../README.md`](../README.md)*
