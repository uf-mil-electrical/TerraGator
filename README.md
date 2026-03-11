# ESP32 nRF24L01 Wireless Communication

This project demonstrates **wireless communication between two ESP32 boards** using nRF24L01 modules.  
One ESP32 acts as a **Transmitter**, sending `"HELLO"` every second.  
The other ESP32 acts as a **Receiver**, printing received messages to the Serial Monitor or PuTTY terminal.

---

## Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [ESP32 Pinout](#esp32-pinout)
---

## Hardware Requirements

- 2x ESP32 boards
- 2x nRF24L01 modules (3.3V)
- Female Jumper wires
- Optional: 10–47µF capacitor for nRF24L01 power stability

---

## ESP32 Pinout for nRF24L01

| nRF24L01 Pin | ESP32 Pin | Function |
|--------------|-----------|---------|
| GND          | GND       | Ground |
| VCC          | 3.3V      | Power (3.3V only!) |
| CE           | GPIO 4    | Chip Enable |
| CSN          | GPIO 5    | SPI Chip Select |
| SCK          | GPIO 18   | SPI Clock |
| MOSI         | GPIO 23   | SPI Master Out (data to nRF) |
| MISO         | GPIO 19   | SPI Master In (data from nRF) |
| IRQ          | Not connected | Optional interrupt pin |



