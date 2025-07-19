# 🧠 Smart RC Motor Controller with Obstacle Avoidance – Arduino Nano

This project intercepts **RC PWM signals**, manipulates them in real-time, and outputs to motor controllers (servos or ESCs) via PWM. Built for **Arduino Nano**, it includes advanced features such as input smoothing, deadzone handling, timeout fail-safe, and **ultrasonic-based obstacle prevention** (e.g., using HC-SR04).

🎥 **Watch it in action on YouTube**: [https://youtube.com/shorts/OADyVq37MFk](https://youtube.com/shorts/OADyVq37MFk)

---

## 🚀 Features

- 🧲 **Reads RC PWM signals** using hardware interrupts (D2/D3)
- ⚙️ **Outputs servo PWM signals** (D9/D10) for motors
- 🧪 **Debug mode**: simulate RC inputs without transmitter
- 🕵️ **Timeout fail-safe**: shuts down motors if input signal is lost
- 🧯 **Deadzone filter**: eliminates jitter near center
- 🧼 **Smoothing**: exponential moving average for clean control
- 🧱 **Obstacle avoidance** using HC-SR04:
  - Detects objects closer than a defined distance
  - Blocks *forward* motion only; *reverse* is always allowed
- 💡 Modular functions and constants for easy tuning

---

## 🧰 Hardware Requirements

- **Arduino Nano** (ATmega328)
- **2x RC input channels** (e.g., from receiver)
- **2x motor drivers or servos** (PWM-controlled)
- **HC-SR04 Ultrasonic Sensor** (optional for blocking)

### Pin Mapping

| Function        | Arduino Pin |
|----------------|-------------|
| RC Input CH1    | D2 (INT0)   |
| RC Input CH2    | D3 (INT1)   |
| Motor Output 1  | D9          |
| Motor Output 2  | D10         |
| HC-SR04 Trig    | A0          |
| HC-SR04 Echo    | A1          |

---

## 🧪 Debug Mode (No Transmitter Needed)

Set the following macro at the top of the sketch:

```cpp
#define DEBUG_MODE 1
