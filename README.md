# Field-Serviceable-Smart-Power-Monitor-using-STM32F103C8T6
Developed a cost-effective energy monitoring device with professional-grade firmware update capability.

---

# Structure
- `docs/`: Documentation and design notes.
  - `notes/`: Development notes and considerations.
  - `references/`: Reference materials and datasheets.
- `project-architecture/`: Overview of the project architecture, containing main driver code structure.
- `project-halartifactspace/`: Hardware Abstraction Layer artifacts generated from STM32CubeIDE & STM32CubeMX to act as a reference for HAL-based implementations.
- `project-obsidianspace/`: Obsidian vault for project notes and documentation.

---

# Overview

This project focuses on creating a smart power monitor using the STM32F103C8T6 microcontroller. The device is designed to be field-serviceable, allowing for easy maintenance and updates in professional settings. Key features include:
- Real-time energy monitoring capabilities.
- Reliable system design for continuous operation with Watchdog & Reset-Clock features.
- Firmware update functionality to ensure the device can be easily updated in the field using XMODEM protocol & CRC checks.

---

# Features
- **Microcontroller**: STM32F103C8T6 (Cortex-M3)
- **Peripherals**:
  - ADC for voltage and current sensing
  - UART for communication and firmware updates
  - GPIO for status indicators and control signals