# 🎡 Theme Park Management System

<p align="center">
  <img alt="Language" src="https://img.shields.io/badge/language-C-blue?style=for-the-badge">
  <img alt="Graphics" src="https://img.shields.io/badge/graphics-Raylib%20%2B%20raygui-ff586e?style=for-the-badge">
  <img alt="Status" src="https://img.shields.io/badge/status-working-ffc83d?style=for-the-badge">
  <img alt="License" src="https://img.shields.io/badge/license-educational-461a52?style=for-the-badge">
</p>

<p align="center">
  A colorful, single-file desktop app that runs a theme park: sell tickets, check guests in at the gate,
  manage rides and food, keep staff information, handle safety emergencies, and print a daily revenue report.
</p>

---

## ✨ Features

| Module | What it does |
| --- | --- |
| 🎟️ **Ticket Counter** | Sells tickets based on age, with day or multi-day passes and an optional wristband. |
| 🚪 **Gate & Security** | Checks a ticket code and password at the gate, plus a metal detector and bag-weight check. |
| 🎢 **Rides** | Five rides, each with its own age, height and weight rules. Wristband holders get one free ride per type. |
| 🍔 **Food Court** | Food and drinks menu with quantities, order summary and running food sales total. |
| 👷 **Employee Information** | Read-only staff table with IDs, ages, salaries and duties. |
| 🛡️ **Safety & Emergency** | Fire alarm check, evacuation plan, exit status, public announcements and medical help. |
| 📊 **Admin Dashboard** | Daily revenue report and a paged list of registered visitors. |
| 🖥️ **Resizable Window** | The layout scales with the window. Press **F11** for fullscreen. |

---

## 🎟️ Ticket Prices

| Category | Age | Base price | Notes |
| --- | --- | --- | --- |
| Toddler | 1 to 3 | Rs. 0 | Gate code `T` |
| Child | 4 to 12 | Rs. 150 | Gate code `C` |
| Adult | 13 to 59 | Rs. 300 | Gate code `A` |
| Senior | 60 to 100 | Rs. 100 | Gate code `S` |

- **Multi-day pass:** +50% of the base price.
- **Wristband:** +Rs. 150.

---

## 🎢 Ride Rules

| Ride | Price | Requirements |
| --- | --- | --- |
| Infant Ride | Rs. 150 | Age 12 months or less, weight 15 kg or less |
| Roller Coaster | Rs. 200 | Age 12 or more, height 140 cm or more |
| Sky Drop Tower | Rs. 200 | Age 12 or more, height 140 cm or more |
| Giant Wheel | Rs. 250 | Open to everyone |
| Dodgem Car | Rs. 250 | Age 5 or more |

With a wristband, the **first ride of each type is free**.

---

## 🍕 Food Court Menu

| Food | Price | Drinks | Price |
| --- | --- | --- | --- |
| Burger | Rs. 250 | Cold Drink | Rs. 100 |
| French Fries | Rs. 100 | Juice | Rs. 70 |
| Pasta | Rs. 300 | Tea | Rs. 60 |
| Small Pizza | Rs. 350 | Water | Rs. 100 |
| Large Pizza | Rs. 550 | | |
| Veg Sandwich | Rs. 150 | | |
| Club Sandwich | Rs. 200 | | |

---

## 🚀 Getting Started

### Requirements

- A C compiler (GCC or MinGW)
- [Raylib](https://www.raylib.com/) version 5.0 or newer
- `raygui.h` (single-header file), placed in the same folder as `park.c`

### Build

**Windows (MinGW):**

```bash
gcc park.c -o park.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

**Linux:**

```bash
gcc park.c -o park -lraylib -lm -lpthread -ldl -lrt -lX11
```

**macOS:**

```bash
gcc park.c -o park -lraylib -framework OpenGL -framework Cocoa -framework IOKit
```

### Optional Sound

Place these files next to the program to enable audio. The app runs fine without them.

- `bgsound.mp3`: background music (loops, plays at 40% volume)
- `click.wav`: sound played on every button click

---

## 🎮 Controls

| Input | Action |
| --- | --- |
| Mouse click | Press buttons and select text boxes |
| Keyboard | Type into the active text box |
| **F11** | Toggle fullscreen |
| Window corner drag | Resize the window (the layout scales automatically) |

---

## 🔐 Access and Passwords

- **Gate passwords** for each ticket code are set in the code (`categoryPassword`).
- **Admin login** uses the password defined as `ADMIN_PASSWORD` in `park.c`. It is currently `YES`, which is for demo use only. Change it before using the app for anything real.

---

## 🛡️ Safety Thresholds

- **Fire:** a smoke level above **50** triggers the fire alert and activates the evacuation plan automatically.
- **Security:** a bag heavier than **15 kg** needs a manual check.

---

## 🧱 Project Structure

```
.
├── park.c        # The whole program (logic + GUI)
├── raygui.h      # raygui single-header library (you provide it)
├── bgsound.mp3   # optional background music
└── click.wav     # optional click sound
```

The code is organized in numbered sections:

1. **Constants and theme** (screen size, colors)
2. **Types** (screens, input fields, visitor and staff structs)
3. **Fixed data** (prices, passwords, menus, staff list)
4. **Global state** (current screen, sales totals, text buffers)
5. **Drawing helpers** (buttons, text boxes, panels)
6. **Business logic** (ticketing, rides, food, safety, visitors)
7. **Screens** (one drawing function per screen)
8. **Assets and main loop**
9. **Resizable window** (canvas scaling and mouse mapping)

---

## 🎨 Customizing the Theme

All colors are defined at the top of `park.c`:

```c
#define COL_BG      CLITERAL(Color){ 255, 243, 224, 255 }  // cream background
#define COL_PANEL   CLITERAL(Color){ 46, 26, 82, 255 }     // night-sky purple panels
#define COL_ACCENT  CLITERAL(Color){ 255, 88, 110, 255 }   // carnival pink-red
#define COL_GOLD    CLITERAL(Color){ 255, 200, 61, 255 }   // ticket gold
```

Change a value and rebuild to restyle the whole app.

---

## 🗺️ Roadmap

- [ ] Save sales and visitor data to a file so it survives restarts
- [ ] Per-visitor ride history
- [ ] Hashed admin password
- [ ] Printable receipts

---

## 📜 License

Made for educational purposes. Feel free to learn from it, modify it, and build on it.

<p align="center">
  Made with 💜 using <b>C</b>, <b>Raylib</b> and <b>raygui</b>
</p>
