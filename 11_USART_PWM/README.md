# Example 11: USART_PWM

This directory provides the architectural layout, header files (.h), and structural interfaces for the USART_PWM application layer.

## 🎯 Architectural Overview
*   Description: Connect LEDs to PWM and control frequency and duty cycle by serial commands.
*   Key Technologies & Drivers: Configures advanced timer channels to generate Pulse Width Modulation (PWM) signals, controlling LED dimming metrics dynamically over UART.
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