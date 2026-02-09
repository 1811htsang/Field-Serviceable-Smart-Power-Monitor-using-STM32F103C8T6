# Field-Serviceable-Smart-Power-Monitor-using-STM32F103C8T6

## Introduction

Developed a cost-effective energy monitoring device with professional-grade firmware update capability.

---

## Structure

- `docs/`: Documentation and design notes.
  - `notes/`: Development notes and considerations.
  - `references/`: Reference materials and datasheets.
- `project-architecture/`: Overview of the project architecture, containing main driver code structure.
- `project-halartifactspace/`: Hardware Abstraction Layer artifacts generated from STM32CubeIDE & STM32CubeMX to act as a reference for HAL-based implementations.
- `project-obsidianspace/`: Obsidian vault for project notes and documentation.

---

## Overview

This project focuses on creating a smart power monitor using the STM32F103C8T6 microcontroller. The device is designed to be field-serviceable, allowing for easy maintenance and updates in professional settings. Key features include:

- Real-time energy monitoring capabilities.
- Reliable system design for continuous operation with Watchdog & Reset-Clock features.
- Firmware update functionality to ensure the device can be easily updated in the field using XMODEM protocol & CRC checks.

---

## Features

- **Microcontroller**: STM32F103C8T6 (Cortex-M3)
- **Peripherals**:
  - SPI for communication with the ADE7758 energy metering IC
  - UART for communication and firmware updates
  - GPIO for status indicators and control signals
  - RCC for reset & clock management
  - IWDG for watchdog functionality
  - I2C for LCD interfacing (optional)
- **Energy Metering**: ADE7758 IC for accurate energy measurement.
- **Firmware Update**: XMODEM protocol with CRC checks for reliable updates.
- **Power Monitoring**: Real-time measurement and reporting of power consumption.

---

## Hardware Components

Note that this sections can be expand with new requirements in the future.

- STM32F103C8T6 Microcontroller
- ADE7758 Energy Metering IC
- LCD 16x2 Display (Optional for user interface)
- Relay 5V for load control
- L7805CV Voltage Regulator
- BC547 Transistor
- MCT2E Optocoupler
- Various resistors, capacitors, diodes, and connectors as per the schematic or PCB design.
