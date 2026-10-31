# Example 02: LED_2

This directory contains a fully functional, open-source bare-metal implementation for the **Nucleo-F746ZG** development board. It serves as a foundational step in the hands-on curriculum of the textbook **"Hands-On Embedded Software Design and Programming"**.

## 🎯 Objective
* **Core Task:** Blink LED using tight loop scanning Timer’s CNT register.
* **Key Concepts Covered:** Replaces crude software delay loops with deterministic hardware timing by polling a hardware Timer's counter register.
* Language/Stack: C (STM32 HAL Layer)

## 🔌 Hardware Setup
* Board: STMicroelectronics Nucleo-F746ZG (Cortex-M7)
* Peripherals Used: Built-in onboard Nucleo components and ST-LINK virtual COM port.

## 🚀 How to Run
1. Open this project folder in your preferred toolchain (e.g., STM32CubeIDE, VS Code with Cortex-Debug, or Make/CMake setup).
2. Connect your Nucleo-F746ZG board via the onboard ST-LINK micro-USB port.
3. Build the project and flash the binary to the target board.
4. Verify operation as outlined in the objective.

---

📖 This project is part of a 21-step progressive curriculum designed for entry-level software engineers and CS/EE students. For deep architectural breakdowns, hardware optimization tips, and step-by-step guidance, check out the full textbook:

👉 Get "Hands - On Embedded Software Design and Programming" on Amazon / Leanpub