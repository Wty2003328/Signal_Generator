# VGA Line Swipe Generator

Arduino-based VGA signal generator creating dynamic color swipe patterns. Demonstrates direct VGA timing control using AVR microcontrollers.

## Features
- **480×480 Resolution** @ 60Hz
- **4 Swipe Directions**:
  - → Right-to-Left
  - ← Left-to-Right
  - ↓ Top-to-Bottom
  - ↑ Bottom-to-Top
- **Customizable Effects**:
  - Adjustable line thickness (0-255 pixels)
  - Configurable animation speed
  - RGB color control
- **Optimized Performance**:
  - 98% CPU-free operation
  - 34 bytes SRAM usage
  - Interrupt-driven timing

## Hardware Setup

### Components
| Component         | Quantity | Value  |
|-------------------|----------|--------|
| Arduino Uno       | 1        | -      |
| DB15 Connector    | 1        | Female |
| Resistors         | 3        | 470Ω   |
| Resistors         | 2        | 68Ω    |
| Breadboard        | 1        | -      |

### Wiring Diagram
| Arduino Pin | Signal  | DB15 Pin | Color  |
|-------------|---------|----------|--------|
| D3          | HSYNC   | 13       | Brown  |
| D4          | Red     | 1        | Red    |
| D5          | Green   | 2        | Green  |
| D6          | Blue    | 3        | Blue   |
| D10         | VSYNC   | 14       | White  |
| GND         | Ground  | 5,6,7,8,10 | Black |

## Installation
1. **Required Library**:
   ```bash
   git clone https://github.com/JChristensen/TimerHelpers.git
   ```
2. **Arduino IDE Setup**:
   - Open `VGA_Line_Swipe/VGA_Line_Swipe.ino`
   - Select board: Arduino Uno
   - Set optimization: `Tools → Optimize → "Smallest Code"`

### Configuration
Modify these constants in the code:
```cpp
// MAIN CONFIGURATION
constexpr SwipeDirection DIRECTION = RIGHT_TO_LEFT;
constexpr int FRAMES_PER_MOVE = 0;   // 0=Max speed
constexpr int LINE_THICKNESS = 5;    // Pixels
constexpr byte LINE_COLOR = 0x70;    // BGR Format

// COLOR PALETTE (BGR4)
#define COLOR_BLACK   0x00
#define COLOR_RED     0x10  // 00010000
#define COLOR_GREEN   0x20  // 00100000
#define COLOR_BLUE    0x40  // 01000000
#define COLOR_YELLOW  0x30  // 00110000
#define COLOR_MAGENTA 0x50  // 01010000
#define COLOR_CYAN    0x60  // 01100000
#define COLOR_WHITE   0x70  // 01110000
```
### Usage
- Upload sketch
- Power Arduino via stable 9V supply
- Connect to VGA display
- Patterns start automatically

#### Control Options:
- Change `DIRECTION` for different swipe patterns
- Modify `LINE_THICKNESS` for effect width
- Adjust `FRAMES_PER_MOVE` (higher = slower)

## Technical Specifications
### Signal Timing
| Parameter            | Value     | Tolerance |
|---------------------|-----------|-----------|
| Vertical Frequency   | 59.94Hz   | ±0.1%    |
| Horizontal Frequency | 31.25kHz  | ±1%      |
| Pixel Clock         | 12.5MHz   | Critical  |
| HSYNC Pulse         | 4μs       | ±0.5μs   |
| VSYNC Pulse         | 64μs      | ±2μs     |

### Resource Usage
| Resource | Used  | Available |
|----------|-------|-----------|
| Flash    | 1,812B| 32KB     |
| SRAM     | 34B   | 2KB      |
| CPU Load | <2%   | -         |

## Troubleshooting
### No Image:
- Verify sync signal continuity
- Check resistor values (68Ω/470Ω)
- Ensure monitor supports `640x480@60Hz`

### Flickering:
```cpp
// Try adjusting these in setup():
OCR1A = 259; → 260  // Vertical
OCR2A = 63; → 64    // Horizontal
```

### Color Issues:
- Swap RGB connections
- Check for resistor mismatch
- Verify `PORTD` configuration (Pins 4-6)
