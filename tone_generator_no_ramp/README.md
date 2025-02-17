# Programmable Tone Generator

Precision audio generator with synchronized LED feedback and digital volume control. Implements frequency cycling with smooth volume ramping effects.

## Features
- **Three Frequency Presets**: 3kHz, 10kHz, 30kHz
- **Dynamic Volume Control**:
  - Digital attenuation via PT2258
  - Smooth volume ramping
  - Configurable base volume levels
- **Visual Feedback**:
  - LED blink synchronization
  - 80-blink cycle with pause
- **Safety Features**:
  - Long-press activation (3s hold)
  - Automatic shutdown after 30 cycles
  - Fail-safe mute on startup

## Hardware Setup

### Components
| Component         | Quantity | Specifications      |
|-------------------|----------|---------------------|
| Arduino Nano      | 1        | ATmega328P          |
| AD9833 Module     | 1        | 0-12.5MHz DDS       |
| PT2258 Module     | 1        | I²C Digital Attenuator |
| Tactile Switch    | 1        | Momentary NO        |
| LED               | 1        | 5mm, 20mA           |
| Resistors         | 2        | 220Ω, 10kΩ         |

### Connection Diagram
| Arduino Pin | Component      | Connection        |
|-------------|----------------|-------------------|
| D2          | AD9833 FSYNC   | Module CS         |
| D8          | LED            | Through 220Ω     |
| D9          | Tactile Switch | Pull-up to 5V     |
| A4 (SDA)    | PT2258 SDA     | I²C Bus           |
| A5 (SCL)    | PT2258 SCL     | I²C Bus           |
| 3.3V        | AD9833 VCC     | Power Supply      |
| GND         | Common Ground  | All modules       |

## Installation
1. **Required Libraries**:
   ```arduino
   // Install via Arduino Library Manager
   #include <AD9833.h>      // Waveform Generator
   #include <PT2258.h>      // Digital Attenuator
   ```
2. **Arduino IDE Setup**:
   - Open `Tone_Generator/Tone_Generator.ino`
   - Select board: Arduino Nano
   - Set I2C speed: `Wire.setClock(400000);`

### Configuration
Modify these constants in the code:
```cpp
// FREQUENCY & VOLUME PRESETS
int frequencyList[] = {3000, 10000, 30000};  // Hz
int volumeList[] = {20, 12, 10};             // Attenuation (0-79)

// TIMING PARAMETERS (ms)
constexpr unsigned long BLINK_ON = 10;
constexpr unsigned long BLINK_OFF = 40;
constexpr unsigned long CYCLE_PAUSE = 5000;
constexpr unsigned LONG_PRESS = 3000;

// OPERATIONAL LIMITS
constexpr int MAX_CYCLES = 30;  // Auto-shutdown threshold
```
### Usage
- Upload sketch
- Power via stable 5V supply

#### Operation modes:
- **Standby Mode:**
  - LED off
  - Output muted
  - Press+hold (3s) to activate
- **Active Mode:**
  - LED blinks 80 times (10ms on/40ms off)
  - Frequency cycle pattern:
    - `3kHz @ -20dB (5s)`
    - `10kHz @ -12dB (5s)`
    - `30kHz @ -10dB (5s)`
  - Auto-shutdown after 30 cycles

## Technical Specifications
### Audio Performance
| Parameter            | Specification         |
|---------------------|---------------------|
| Frequency Range     | 1Hz-12.5MHz         |
| Frequency Accuracy  | ±0.1%              |
| Volume Control      | 0-79dB attenuation  |
| THD+N               | <0.1% @ 1V RMS      |

### System Characteristics
| Feature           | Value           |
|-------------------|-----------------|
| I2C Speed        | 400kHz         |
| Current Draw     | 45mA (active)  |
| Cycle Duration   | 15s per frequency |
| Button Debounce  | 50ms           |

## Troubleshooting
### No Sound Output:
- Verify AD9833 power (3.3V)
- Check FSYNC connection (D2)
- Confirm PT2258 initialization:
```cpp
if(!pt2258.begin()) Serial.println("PT2258 Error");
```

### Volume Issues:
```cpp
// Valid attenuation range: 0-79
pt2258.attenuation(1, 20);  // Channel 1, -20dB
```

### LED Not Blinking:
- Check LED polarity
- Verify 220Ω current-limiting resistor
- Test with basic blink sketch

## Contributing
1. Fork repository
2. Create feature branch (`git checkout -b feature/improvement`)
3. Commit changes (`git commit -am 'Add audio feature'`)
4. Push to branch (`git push origin feature/improvement`)
5. Open Pull Request

### ⚠️ Important Notes:
- Use linear power supply for clean audio
- Maintain short signal paths (<10cm)
- Add 100nF decoupling capacitors near ICs
- Consider logic level shifter for 3.3V/5V conversion
