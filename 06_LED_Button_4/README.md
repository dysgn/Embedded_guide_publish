# Example 06: LED_Button_4

This directory contains a fully functional, open-source bare-metal implementation for the **Nucleo-F746ZG** development board. It serves as a foundational step in the hands-on curriculum of the textbook **"Hands-On Embedded Software Design and Programming"**.

## 🎯 Objective
* **Core Task:** Same as LED_Button_3 + add SysTick counter for button debouncing.
* **Key Concepts Covered:** Solves real-world mechanical contact bounce by introducing a software debouncing algorithm paced by the ARM Cortex-M7 SysTick timer.
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