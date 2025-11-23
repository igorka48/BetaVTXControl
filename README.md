# BetaVTXControl

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/igorka48/library/BetaVTXControl.svg)](https://registry.platformio.org/libraries/igorka48/BetaVTXControl)

ESP32 Arduino library for controlling VTX (Video Transmitter) modules via **SmartAudio** and **TRAMP** protocols. Based on [Betaflight](https://github.com/betaflight/betaflight) and [esp-fc](https://github.com/rtlopez/esp-fc) implementations.

Optimized for **PlatformIO** development.

## Features

- **SmartAudio** protocol support (v1, v2, v2.1) - fixed 4800 baud
- **TRAMP** protocol support - fixed 9600 baud
- Frequency control (5000-5999 MHz)
- Power level control (25mW - 800mW typical)
- Pit mode support
- **TX-only mode** (no RX needed, as per esp-fc)
- Dummy byte transmission for UART stabilization
- Non-blocking operations
- Manual protocol selection (VTX_PROTOCOL_SMARTAUDIO / VTX_PROTOCOL_TRAMP)
- CRC validation for SmartAudio
- Checksum validation for TRAMP
- Thread-safe (FreeRTOS compatible)

## Installation

### PlatformIO (Recommended)

**Method 1: From GitHub**

Add to your `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    https://github.com/igorka48/BetaVTXControl.git
```

**Method 2: Local Development**

Copy the library to your project's `lib/` directory.

### Arduino IDE (Legacy)

1. Download the latest release
2. Sketch → Include Library → Add .ZIP Library
3. Select the downloaded file

**Note:** Examples are provided as `.ino` files for Arduino IDE compatibility, but `#include <Arduino.h>` is included for PlatformIO.

## Hardware Connection

**Important:** Only TX pin (GPIO 16) is needed - VTX control wire connects to TX only.

### Wiring Diagram

```
ESP32 GPIO 16 (TX) ────── VTX Control Wire (SmartAudio/TRAMP)
ESP32 GND ───────────────── VTX GND
```

**Notes:**
- Both protocols use half-duplex UART on a single wire
- TX-only mode: Commands sent, no response expected
- SmartAudio: 4800 baud, 8N2
- TRAMP: 9600 baud, 8N1
- Check VTX voltage level (3.3V or 5V) - use level shifter if needed

## Usage

```cpp
#include <BetaVTXControl.h>

#define VTX_TX_PIN 16  // ESP32 GPIO pin for VTX control

BetaVTXControl vtx(VTX_PROTOCOL_SMARTAUDIO); // or VTX_PROTOCOL_TRAMP

void setup() {
  Serial.begin(115200);
  
  if (vtx.begin(&Serial2, VTX_TX_PIN)) {
    vtx.setFrequency(5732);       // R3 (Raceband 3)
    delay(300);                   // IMPORTANT: Wait between commands
    vtx.setPower(200);            // 200mW
    delay(300);
    vtx.setPitMode(false);
    delay(300);
  }
}

void loop() {
  delay(100);  // TX-only: configure once in setup()
}
```

**Important Notes:**
- **SmartAudio**: Power is specified in mW but converted to device index (0-4)
- **TRAMP**: Power is sent directly in mW
- Always add 300ms delay between commands to ensure VTX processes each one
- Some VTX devices may require longer delays (500ms+)

## API Reference

### Initialization

```cpp
BetaVTXControl vtx(VTX_PROTOCOL_SMARTAUDIO);  // or VTX_PROTOCOL_TRAMP
bool begin(HardwareSerial* serial, uint8_t txPin = 16);
```

### Configuration Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `setFrequency(freq)` | `uint16_t freq` | Set frequency in MHz (5000-5999) |
| `setPower(power)` | `uint16_t power` | Set power in mW (25, 200, 400, 600, 800) |
| `setPitMode(enable)` | `bool enable` | Enable/disable pit mode (low power) |

**Note:** TX-only mode - commands are sent immediately, no response expected. Add 300ms delay between commands.

### Direct Protocol Access

```cpp
#include <SmartAudio.h>
SmartAudioVTX vtx;
vtx.begin(&Serial2, 16);  // TX pin

#include <TRAMP.h>
TrampVTX vtx;
vtx.begin(&Serial2, 16);  // TX pin
```

## Protocol Details

| | SmartAudio | TRAMP |
|---|------------|-------|
| **Baudrate** | 4800 (8N2) | 9600 (8N1) |
| **Validation** | CRC8 (0xD5) | Checksum |
| **Packet Start** | `0xAA 0x55` | `0x0F` |
| **Versions** | v1, v2, v2.1 | - |

See [frequency tables](docs/FREQUENCIES.md) for channel mappings.

## Troubleshooting

**VTX not responding:**
- Check wiring (TX on GPIO 16, common GND)
- Verify VTX is powered
- Check voltage levels (use level shifter for 5V VTX)
- Try opposite protocol (SmartAudio ↔ TRAMP)

**Commands ignored:**
- VTX may be in race lock mode
- Add 300ms delays between commands
- Power cycle VTX

**Power commands not working (SmartAudio):**
- SmartAudio uses power index (0-4), not direct mW
- Different VTX models have different power tables
- Try `setPowerByIndex(0)` through `setPowerByIndex(4)` to find working values
- Example: `vtx.setPowerByIndex(2)` might be 400mW on your VTX

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push and create a Pull Request

## License

MIT License - see [LICENSE](LICENSE) file for details.

Based on Betaflight VTX implementation (GPL-3.0).

## Credits

- [Betaflight](https://github.com/betaflight/betaflight) - Original protocol implementation
- [esp-fc](https://github.com/rtlopez/esp-fc) - ESP32 flight controller implementation
- SmartAudio protocol by TBS (Team BlackSheep)
- TRAMP protocol by ImmersionRC

## Support

- Issues: [GitHub Issues](https://github.com/igorka48/BetaVTXControl/issues)
- Discussions: [GitHub Discussions](https://github.com/igorka48/BetaVTXControl/discussions)
