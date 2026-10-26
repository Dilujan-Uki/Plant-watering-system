# Progress Log — Daily Build Diary

> A day-by-day, Markdown-tracked record of this Plant Watering System build.  
> Each day is a **self-contained markdown file** — what was planned, what was done, what was learned, what’s next.

---

## Why Daily Logs?

- **Accountability** — visible progress = steady momentum
- **Debugging** — when something breaks, you know exactly what changed
- **Portfolio** — recruiters love “show your work” over “I built a thing”
- **Reproducibility** — anyone can follow the same steps and get the same result
- **GitHub activity** — each log is a commit → green contribution graph

---

## Index

| # | Date | Focus | Status | Link |
|---|------|-------|--------|------|
| **01** | **2026-10-26** | **Research & Inspiration: studied 2 videos (Arduino baseline + ESP32/Blynk), learned IoT basics, gathered ESP32 materials** | ✅ Done | [day-01.md](day-01.md) |
| 02 | — | Breadboard wiring + first ADC test (dry/wet calibration) | ⬜ Next | `day-02.md` (use TEMPLATE) |
| 03 | — | Blynk cloud setup + live monitoring | ⬜ Planned | — |
| 04 | — | Pump + relay integration + hysteresis & safety caps | ⬜ Planned | — |
| 05 | — | Local display (OLED/LCD) + manual button | ⬜ Planned | — |
| 06 | — | Environment compensation (DHT) + field tuning | ⬜ Planned | — |
| 07 | — | Enclosure + cable management + reservoir interlock | ⬜ Planned | — |
| 08 | — | Deep-sleep power saving + long-run test (24–48 h) | ⬜ Planned | — |
| 09 | — | Demo video + documentation polish + final push | ⬜ Planned | — |

> Add a new row every day. Keep the table in `README.md` in sync.

---

## How to Write a Daily Log (60 seconds)

1. **Copy the template:**

   ```bash
   cp progress/TEMPLATE.md progress/day-02.md
   ```

2. **Fill it** — sections are:

   | Section | What to write (2–3 bullets is enough) |
   |---------|----------------------------------------|
   | **Goal** | What you planned to achieve |
   | **What I Did** | What actually happened (with links/photos) |
   | **Materials / Tools Used** | Anything new purchased or reused |
   | **Challenges & Fixes** | Errors + how you solved (or didn’t yet) |
   | **Learning** | One-sentence takeaway |
   | **Next Steps** | What tomorrow will focus on |
   | **Time Spent** | Rough hours |
   | **Media** | Photos, screenshots, serial logs |

3. **Commit & push:**

   ```bash
   git add progress/day-02.md
   git commit -m "docs: add day 02 progress — breadboard wiring & ADC test"
   git push origin arena/360e0168-plant-watering-system
   ```

4. **Update this index + main README’s Progress Log table.**

### Naming

- Format: `day-02.md` or `day-02-2026-10-27.md` (both OK — keep consistent after you pick one)
- This repo uses `day-01.md` (short) — prefer short unless you need date disambiguation.

### Photos

- Store under `docs/images/` — e.g., `docs/images/day-02-wiring.jpg`
- Embed: `![Wiring day 2](../docs/images/day-02-wiring.jpg)`

---

## Template

- **Blank template:** [`TEMPLATE.md`](TEMPLATE.md) — copy for each new day

---

## Tips

- **Done > Perfect.** A 10-line honest log beats no log.
- **Log blockers too.** “Tried X, failed because Y, will try Z” is valuable.
- **Link commits.** At the bottom of a day log, add: `Commit: abc1234 — message`
- **No back-dating.** Write the day’s log the same day; note if you missed a day.

---

## Back

- **Main README:** [`../README.md`](../README.md)
- **Today’s log:** [`day-01.md`](day-01.md)
