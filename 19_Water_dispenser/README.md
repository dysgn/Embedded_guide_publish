# Example 19: Water_dispenser

This directory provides the architectural layout, header files (.h), and structural interfaces for the Water_dispenser application layer.

## 🎯 Architectural Overview
*   Description: Controller application with shutoff valve, pump and flow meter implemented in controller; Dispenser_Host Python application on the host side.
*   Key Technologies & Drivers: Builds a complete physical industrial emulator containing a pump, valve, and flow sensor. Interfaces natively with a master control application on Python over UART.
*   Language/Stack: C/C++ (Embedded Target Firmware via ST HAL), Python (Host Utility Scripting)

## 📁 Repository Contents
*   include/ / headers/: Architectural interfaces and driving configurations.
*   src/: [Implementation files (.c/.cpp) are excluded from the public repository layout]

## ⚠️ Textbook Exclusive Content
The complete source code implementation, step-by-step library configuration, thread-safety logic, and network protocol breakdown for this advanced project are exclusive to the textbook:

### 📘 "Hands - On Embedded Software Design and Programming"

This textbook is specifically written for entry-level software engineers and CS/EE students looking to bridge the gap between simple sandbox testing and professional, rugged production-level software design on STMicroelectronics platforms.

## What you will learn in this specific chapter:
1.  Advanced architectural patterns for stacking STM32 HAL layers.
2.  Practical debugging techniques for cross-platform target-to-host communications.
3.  Writing bulletproof host scripts in Python to analyze and command target firmware systems.

👉 Purchase the Book & Unlock All 21 Source Code Projects Here