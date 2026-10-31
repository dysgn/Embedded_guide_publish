# Hands-On Embedded Software Design and Programming
### Build 21 Practical Applications Using C, C++, and Python on STM32F7\n
The repository outlines 21 progressive embedded systems projects, starting from basic bare-metal LED blinking and GPIO manipulation to advanced communication protocols like SPI with DMA, I2C, FreeRTOS with TCP/IP, and a custom bootloader ecosystem. All target examples are developed for the ARM Cortex-M7 architecture on the **STMicroelectronics Nucleo-F746ZG** board.\n
This repository serves as the official source code companion to the textbook **\"Hands-On Embedded Software Design and Programming\"**, specifically tailored for entry-level software engineers and CS/EE students.\n
---\n
## 🗺️ Course Roadmap & 21 Project Directory\n
*Examples 1–6 are fully open-source. Examples 7–21 include architectural definitions, with their complete step-by-step implementations exclusive to the textbook.\n
### 🟢 Part 1: Foundations & Bare-Metal Fundamentals (Open Source)
*   **[01_LED_1](./01_LED_1):** Just blink LED with tight loop counting for delay.
*   **[02_LED_2](./02_LED_2):** Blink LED using tight loop scanning Timer’s CNT register.
*   **[03_LED_Button](./03_LED_Button):** Same as LED_2 + switching LED color on a button press.
*   **[04_LED_Button_2](./04_LED_Button_2):** Same as LED_Button, button is using interrupt instead of polling.
*   **[05_LED_Button_3](./05_LED_Button_3):** Both blinking and button handled in interrupt, main loop is empty.
*   **[06_LED_Button_4](./06_LED_Button_4):** Same as LED_Button_3 + add SysTick counter for button debouncing.
\n### 🟡 Part 2: Serial Communications & Peripherals (Book Companion)
*   **[07_USART_Hello](./07_USART_Hello):** Send “Hello, world!” once through UART.
*   **[08_USART_Hello_2](./08_USART_Hello_2):** Send “Hello, world!” through UART each time LF is received through UART.
*   **[09_USART_Hello_3](./09_USART_Hello_3):** Same as UART_Hello_2, but use non-blocking UART functions.
*   **[10_USART_LED](./10_USART_LED):** Control blinking of LEDs through serial port commands.
*   **[11_USART_PWM](./11_USART_PWM):** Connect LEDs to PWM and control frequency and duty cycle by serial commands.
*   **[12_USART_ADC](./12_USART_ADC):** Make ADC scans paced by receiving LF from serial.
*   **[13_USART_PWM_ADC](./13_USART_PWM_ADC):** Control PWM period by serial command, and PWM duty cycle (LED brightness) by ADC (dials).
\n### 🔵 Part 3: Advanced Protocols & DMA (Book Companion)
*   **[14_SPI_LEDStrip](./14_SPI_LEDStrip):** Control LED strip through SPI interface.
*   **[15_SPI_LEDStrip_2](./15_SPI_LEDStrip_2):** Same as SPI_LEDStrip, using DMA for SPI.
*   **[16_SPI_Gyro](./16_SPI_Gyro):** Periodic reading of accelerometer over SPI.
*   **[17_SPI_Gyro_2](./17_SPI_Gyro_2):** Same as SPI_Gyro, but using array read of registers and DMA for SPI.
*   **[18_I2C_ADC_LED](./18_I2C_ADC_LED):** Use potentiometers to control LED stick color/brightness over I2C bus.
\n### 🔴 Part 4: Full-Stack Systems & RTOS (Book Companion)
*   **[19_Water_dispenser](./19_Water_dispenser):** Controller application with shutoff valve, pump and flow meter.
*   **[20_Water_dispenser_RTOS](./20_Water_dispenser_RTOS):** Controller application running on FreeRTOS and using TCP/IP over Ethernet.
*   **[21_Water_dispenser_Bootloader](./21_Water_dispenser_Bootloader):** Custom bare-metal bootloader application.
*   **[21_Water_dispenser_LoadableApp](./21_Water_dispenser_LoadableApp):** Modified application to work with bootloader.
*   **[21_Water_dispenser_Host_LoadApp](./21_Water_dispenser_Host_LoadApp):** Python utility to reprogram the board.
\n---\n
## 🛠️ Hardware & Toolchain Target
*   **Development Board:** STMicroelectronics Nucleo-F746ZG (ARM Cortex-M7 @ 216 MHz, 1MB Flash, 320KB SRAM)
*   **Languages:** C, C++ (Target firmware), Python (Host PC script utilities)
*   **Production-Ready HAL:** Built entirely using the industry-standard STMicroelectronics HAL library.\n
---\n
## 📘 Unlock the Full Text & Explanations
To master professional firmware design patterns, step-by-step HAL API breakdowns, full source code implementations, and comprehensive instructions on setting up your environment from scratch:\n
**[👉 Get your copy of \"Hands-On Embedded Software Design and Programming\" here](https://www.amazon.com/dp/B0HPWQXPX1)**