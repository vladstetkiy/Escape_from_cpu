# Escape_from_cpu
This project is a 3D maze with puzzle elements, implemented on Arduino using raycasting technology (similar to that used in Wolfenstein 3D). The player must find and ‘fix’ damaged sections of walls by answering programming and computer technology questions.

## ✨ Features

- **Raycast 3D rendering** optimized for Arduino
- **Dynamic maze generation** with breakable walls
- **Progress tracking** with visual indicators
- **Joystick-controlled** gameplay

## 🛠 Hardware Requirements

| Component              | Specification           |
|------------------------|-------------------------|
| Microcontroller        | Arduino Uno             |
| Display                | ST7789 (240x320)        |
| Input                  | 2x Analog Joysticks     |
| Power                  | 5V USB/DC               |

## 📥 Installation Guide

### 1. Hardware Setup

#### Wiring Diagram
```plaintext
ST7789 Display:
- CS    → Pin 10
- DC    → Pin 8
- RST   → Pin 9
- MOSI  → Pin 11
- SCK   → Pin 13
- VCC   → 5V
- LED   → 5V
- GND   → GND

Movement Joystick:
- VRx   → A3
- VRy   → A2
- SW    → Pin 2 (PULLUP)

Camera Joystick:
- VRx   → A1
- VRy   → A0
- SW    → Pin 3 (PULLUP)
```
### 2. Software Setup

Download the .ino file to your pc and upload the sketch to the microcontroller using arduino ide
