# Arduino Hardware Projects Suite

 
*Dual-project suite demonstrating VGA video generation and precision audio synthesis*

## Overview
This repository contains two  Arduino projects:

1. **VGA Line Swipe Generator** - Direct VGA signal generation with dynamic patterns
2. **Programmable Tone Generator** - Precision audio synthesis with digital volume control

Designed for hardware enthusiasts and educators to explore:
- Real-time signal generation
- Low-level timing control
- Hardware interfacing techniques
- Embedded system optimization

## Project Matrix

| Feature                | VGA Generator          | Tone Generator         |
|------------------------|------------------------|------------------------|
| Core Function          | Video Pattern Generation | Audio Frequency Synthesis |
| Resolution             | 480×480 @ 60Hz         | 0.1Hz Frequency Steps  |
| Control Interface      | Hardware-only          | Tactile Switch + LED   |
| Output Type            | Analog RGB             | Line-level Audio       |
| Key Components         | Timer ICs, Resistors   | AD9833, PT2258 ICs     |
| Power Requirements     | 9V DC                  | 5V DC                  |
| Code Complexity        | 1.8KB Flash            | 2.1KB Flash            |

## Hardware Requirements

### Common Components
- Arduino Uno/Nano
- Breadboard
- Jumper wires
- Stable DC power supply

### VGA Project Specific
| Component              | Quantity | Purpose               |
|------------------------|----------|-----------------------|
| DB15 Connector         | 1        | VGA Output            |
| 470Ω Resistors         | 3        | RGB Current Limiting  |
| 68Ω Resistors          | 2        | Sync Signal Termination |

### Audio Project Specific
| Component              | Quantity | Purpose               |
|------------------------|----------|-----------------------|
| AD9833 Module          | 1        | DDS Waveform Generation |
| PT2258 Module          | 1        | Digital Attenuation   |
| 220Ω Resistor          | 1        | LED Current Limiting  |
| 10kΩ Resistor          | 1        | Button Pull-up        |

