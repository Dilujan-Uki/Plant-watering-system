# References — Videos, Articles & Repos

> Every source that informed this build — watch/read in this order.

---

## 1. Primary Video Inspirations (Day 01 Research)

### 1.1 DIY Automatic Plant Watering System Using Arduino — Simple Circuits

- **Link:** https://www.youtube.com/watch?v=9x3YIiROZ3s
- **Channel:** Simple Circuits — published 2026-01-31, 15.5k views
- **Length:** 04:35
- **Stack:** Arduino Uno + soil moisture sensor + 5 V relay + DC water pump + battery
- **Circuit diagram & code:** https://simplecircuitslol.blogspot.com/2026/02/plant-watering-system-using-arduino.html
- **What it taught:**
  - Minimal viable circuit: sensor → Arduino ADC → relay → pump
  - Relay wiring: **pump + → relay NO**, **battery + → relay COM**, shared GND; VCC/GND of relay to Arduino 5V/GND, IN to D7
  - Separation of supplies: **battery for pump only**; Arduino from USB/5 V adapter — avoids brown-out/burn
  - Good starter demo, but **Arduino has no Wi-Fi** → not IoT by itself
- **Gap we improve:** Move to ESP32 for native Wi-Fi + Blynk cloud.

### 1.2 How to make a plant watering system with ESP32 board and Blynk app — SriTu Hobby

- **Link:** https://www.youtube.com/watch?v=Ix3a3ThsHfA
- **Channel:** SriTu Hobby — published 2023-05-23, 238k views
- **Length:** 06:03
- **Stack:** ESP32 DevKit + soil moisture sensor + 16×2 LCD + I2C module + 1-ch relay + breadboard + 18650 battery holder + Blynk app + mini pump & pipe
- **Companion article + source code & wiring:** https://srituhobby.com/how-to-make-a-plant-watering-system-with-esp32-board-and-blynk-app/
- **Store BOM (SriTu Hobby shop):**
  1. ESP32 adapter/board ×1
  2. Soil moisture sensor ×1
  3. LCD 16×2 ×1
  4. I2C module ×1
  5. 1-channel 5 V relay ×1
  6. Breadboard ×1
  7. Jumper wires
  8. 18650 battery holder ×1
  9. Mini water pump + pipe
  10. Li-ion batteries (18650) ×2
- **What it taught:**
  - **ESP32-native IoT build** — direct Wi-Fi to Blynk
  - Choice of **capacitive sensor** (longer life vs resistive)
  - Local feedback via LCD (mirrors cloud state) — great for demos without phone
  - Blynk virtual pins for moisture %, pump state, mode
  - **Primary blueprint this repo follows** (ESP32 variant)

### 1.3 How to make a plant watering system with the Nodemcu ESP8266 board and the new Blynk update — SriTu Hobby (Day 02 Focus)

- **Link:** https://www.youtube.com/watch?v=WLCR0r7rYi8
- **Channel:** SriTu Hobby — published 2022-07-23, 292k views
- **Length:** 08:03
- **Stack:** NodeMCU ESP8266 + soil moisture sensor + 1-ch 5V relay + 16×2 LCD + I2C backpack + breadboard + 9V battery/clip + mini pump/pipe + new Blynk 2.0 (blynk.cloud:80)
- **Companion article + source + wiring (full):** https://srituhobby.com/how-to-make-a-plant-watering-system-with-the-nodemcu-esp8266-board-and-the-new-blynk-update/
- **Code highlights:**
  ```cpp
  #define sensor A0     // ESP8266 sole ADC
  #define waterPump D3  // relay IN — active-LOW
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  BlynkTimer timer;
  BLYNK_WRITE(V1){ Relay=param.asInt(); digitalWrite(waterPump, Relay?LOW:HIGH); }
  void soilMoistureSensor(){ int v=analogRead(sensor); v=map(v,0,1024,0,100); v=(v-100)*-1; Blynk.virtualWrite(V0,v); }
  // Blynk flow: Template → Datastreams V0 (moisture 0-100), V1 (pump 0/1) → Web Dashboard Gauge+Button → Auth token
  ```
- **What it taught (Day 02 verified):**
  - **New Blynk 2.0 workflow** — blynk.cloud Template + Datastreams + Web Dashboard + Mobile parity (replaces old Blynk).
  - **Relay as Blynk switch pattern** — `BLYNK_WRITE(V1)` → `digitalWrite(relay, ...)` → pump; LCD mirrors `Motor is ON/OFF`.
  - **Blink & I2C LCD check pattern** — upload Blink (LED_BUILTIN/GPIO2/D4), scan I2C `0x27`, call `lcd.init()/lcd.backlight()/lcd.print()` before integrating.
  - **Breadboard basics** — VIN/GND power rails, center trench for NodeMCU, M-M vs M-F jumpers.
  - **Pump power hygiene** — pump on separate 9V/5V PSU with common GND + relay NO/COM; MCU USB only for logic.
  - **ESP8266 vs ESP32 takeaway:** ESP8266 one ADC (`A0`) is enough for single pump; ESP32 multi-ADC needed for expansion; code ports with pin renames.
- **Deep dive in this repo:** [`docs/videos/02-nodemcu-esp8266-blynk-update.md`](videos/02-nodemcu-esp8266-blynk-update.md)

> **Attribution note:** Circuit ideas and component selections above are credited to the original creators. This repo is an *original documentation + daily log* of rebuilding with ESP32/ESP8266, not a verbatim copy of their code.

---

## 2. Written Tutorials & Guides (used to design docs)

- **ESP32IO — Automatic Irrigation System:** https://esp32io.com/tutorials/esp32-automatic-irrigation-system
  - Clear BOM for capacitive sensor + relay + 12 V pump + 12 V adapter + breadboard/jumpers
- **Nodrix — Build an ESP32 automatic plant watering system:** https://nodrix.live/guides/esp32-automatic-plant-watering
  - Best single-table wiring: sensor AOUT→GPIO34, relay IN→GPIO26, separate 5 V supply, flyback diode, active-LOW note
- **IoT Circuit Hub — Indoor Plant Watering System (ESP32):** https://iotcircuithub.com/indoor-plant-watering-system-esp32/
  - DHT11 + OLED variant, pin map (D34 sensor, D25 pump, D26 buzzer), threshold hysteresis (`moistPerLow=20`, `moistPerHigh=80`)
- **Zbotic — Smart Irrigation System with ESP32 (multi-zone):** https://zbotic.in/smart-irrigation-system-with-esp32-auto-watering-plants/
  - Multi-zone wiring table (GPIO34/35 sensors, GPIO25/26 relays, DS3231 on I2C, DHT22 on GPIO4)
- **CircuitDigest — Smart Irrigation with ESP32 + Blynk:** https://circuitdigest.com/microcontroller-projects/smart-irrigation-system-using-esp32-and-blynk-app
  - Blynk code explanation, RGB LED on GPIO21, DHT22 on GPIO4, soil on GPIO35, water level on GPIO34, relay on GPIO5
- **Hackatronic — IoT-Based Irrigation (ESP32):** https://www.hackatronic.com/smart-iot-based-automatic-irrigation-system-using-esp32/
  - Pin table: sensor GPIO34, relay GPIO2, VCC 3.3/5 V, GND common
- **ShillehTek — ESP32 Capacitive Soil Moisture: Smart Auto Watering:** https://shillehtek.com/blogs/news/esp32-capacitive-soil-moisture-smart-auto-watering
  - Concise wiring: sensor VCC→3.3V, GND→GND, AOUT→GPIO34; relay IN→GPIO26, pump via NO/COM

---

## 3. What is IoT? — Background Reading

- **IRJMETS Paper (Nov 2024) — IoT-Based Plant Watering System:** https://www.irjmets.com/uploadedfiles/paper/issue_11_november_2024/64463/final/fin_irjmets1732559499.pdf
  - Definition: soil/temp/humidity sensing → threshold → pump → cloud dashboard + manual override
- **E3S Conferences — IoT & Cloud Based Sustainable Smart Irrigation:** https://www.e3s-conferences.org/articles/e3sconf/pdf/2024/02/e3sconf_icregcsd2023_01026.pdf
  - Layers: sensors → gateway (NodeMCU/ESP32) → cloud/control → mobile/web app
- **Microsoft IoT-For-Beginners — Automated Plant Watering (Lesson 3):** https://github.com/microsoft/IoT-For-Beginners/blob/main/2-farm/lessons/3-automated-plant-watering/README.md
  - Relay control, MQTT, sensor/actuator timing — why watering needs time-aware logic
- **Nature — Intelligent Auto Irrigation with Fuzzy Control:** https://www.nature.com/articles/s41598-025-98137-2
  - Advanced: fuzzy logic irrigation decisions + energy-aware routing
- **Gardens Insider — IoT in Gardening:** https://gardeninsider.org/gardening/technology/iot-gardening-introduction/

---

## 4. GitHub Repos — Professional README Inspiration

These repos were studied for *how* to write great hardware/IoT READMEs (badges, tables, wiring diagrams, progress logs):

- **Chiara2804/Plant-Watering-System:** ESP32 + DHT22 + EMA filtering + OLED + Blynk + Google Sheets — excellent wiring tables & BOM: https://github.com/Chiara2804/Plant-Watering-System
- **EricAbishekh24/IDRPT — Smart Plant Watering System:** Hardware table + pin map (GPIO34 sensor, GPIO26 relay): https://github.com/EricAbishekh24/IDRPT-ASSIGNMENT-2-SMARTPLANT-WATERING-SYSTEM
- **Malathi014/smart-plant-watering-esp32:** LED states (GPIO18/19/21) + buzzer on GPIO23: https://github.com/Malathi014/smart-plant-watering-esp32
- **surj10/self-watering-plant-system:** Reservoir sensing + Blynk V0/V1/V2 + threshold potentiometer on GPIO35: https://github.com/surj10/self-watering-plant-system
- **Md-Abrar-Hossain/Smart-Plant-watering-system-IoT- (ESP32 + DHT11 + Blynk auto/manual):** https://github.com/Md-Abrar-Hossain/Smart-Plant-watering-system-IoT-
- **ChandanaAshok06/Smart_Irrigation_ESP32 (ADC1 warning — ADC2 conflicts with Wi-Fi):** https://github.com/ChandanaAshok06/Smart_Irrigation_ESP32

### README Craft (general)

- **banesullivan/README — How to write a good README:** https://github.com/banesullivan/README
- **GitHub Docs — Create README:** https://docs.github.com/en/copilot/tutorials/customization-library/prompt-files/create-readme
- **Professional README Guide (Bootcamp):** https://coding-boot-camp.github.io/full-stack/github/professional-readme-guide/
- **DEV — README Best Practices (60k+ stars):** https://dev.to/iris1031/github-readme-best-practices-how-to-write-a-readme-that-gets-stars-2gb2

---

## 5. Datasheets & Platform Docs

- **Espressif ESP32 Datasheet & ADC/Wi-Fi notes:** https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
- **Blynk IoT Docs — ESP32 Quickstart & Virtual Pins:** https://docs.blynk.io/
- **Capacitive Soil Moisture Sensor v1.2/v2.0 pinout (ESP32IO):** https://esp32io.com/tutorials/esp32-soil-moisture-sensor
- **Relay Module (1-ch, 5 V, active-LOW) spec — typical:** VCC 5 V, IN 3.3 V-logic compatible, COM/NO/NC 10 A/250 V AC, 10 A/30 V DC

---

## 6. Citation Style

When adding new sources to docs, use:

```md
- **Short Title — Site/Author:** https://example.com — one-line what it taught
```

For academic papers, include title + year + DOI link.

---

## Next

- Back to **README** → [../README.md](../README.md)
- **Overview (IoT primer)** → [overview.md](overview.md)
- **BOM** → [bill-of-materials.md](bill-of-materials.md)
