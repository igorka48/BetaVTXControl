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

**Important:** GPIO 16 and 17 must be soldered together and connected to VTX control wire.

### Wiring Diagram

```
ESP32 GPIO 16 (TX) ──┐
                     ├──── VTX Control Wire (SmartAudio/TRAMP)
ESP32 GPIO 17 (RX) ──┘
ESP32 GND ────────────── VTX GND
```

**Notes:**
- Both protocols use half-duplex UART on a single wire
- Solder GPIO 16 and 17 together for proper operation
- SmartAudio: 4800 baud, 8N2
- TRAMP: 9600 baud, 8N1
- TX-only mode: Commands sent, no response expected
- Check VTX voltage level (3.3V or 5V) - use level shifter if needed

## Usage

### Basic Example (TX-only mode)

```cpp
#include <BetaVTXControl.h>

// Select protocol: VTX_PROTOCOL_SMARTAUDIO or VTX_PROTOCOL_TRAMP
BetaVTXControl vtx(VTX_PROTOCOL_SMARTAUDIO);

void setup() {
  Serial.begin(115200);
  
  // Initialize on Serial2 (GPIO 16 & 17 soldered together)
  if (vtx.begin(&Serial2, VTX_DEFAULT_TX_PIN, VTX_DEFAULT_RX_PIN)) {
    Serial.println("VTX initialized!");
    
    // Configure VTX (commands sent immediately)
    vtx.setFrequency(5732);  // R1 (Raceband 1)
    delay(300);
    
    vtx.setPower(200);       // 200mW
    delay(300);
    
    vtx.setPitMode(false);   // Pit mode OFF
    delay(300);
    
    Serial.println("VTX configured!");
  }
}

void loop() {
  vtx.update();
  
  // Periodically send commands to maintain state
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 5000) {
    vtx.setFrequency(5732);
    vtx.setPower(200);
    lastUpdate = millis();
  }
  
  delay(10);
}
```

### SmartAudio Direct Example

```cpp
#include <SmartAudio.h>

SmartAudioVTX vtx;

void setup() {
  Serial.begin(115200);
  
  // Initialize on Serial2 (GPIO 16/17)
  if (vtx.begin(&Serial2, 16, 17)) {
    Serial.println("SmartAudio VTX initialized!");
  }
}

void loop() {
  vtx.update(); // Call regularly for command queue processing
  
  // Set frequency to Raceband Channel 1 (5658 MHz)
  vtx.setFrequency(5658);
  delay(300);
  
  // Set power to 200mW
  vtx.setPower(200);
  delay(300);
  
  // Enable pit mode
  vtx.setPitMode(true);
  delay(300);
}
```

### TRAMP Direct Example

```cpp
#include <TRAMP.h>

TrampVTX vtx;

void setup() {
  Serial.begin(115200);
  
  // Initialize on Serial2
  if (vtx.begin(&Serial2, 16, 17)) {
    Serial.println("TRAMP VTX initialized!");
  }
}

void loop() {
  vtx.update();
  
  if (vtx.isReady()) {
    // Set frequency
    vtx.setFrequency(5740);
    
    // Set power level
    vtx.setPower(400); // 400mW
    
    // Get temperature
    uint16_t temp = vtx.getTemperature();
    Serial.printf("VTX Temperature: %d°C\n", temp);
  }
  
  delay(100);
}
```

## API Reference

### Common Methods (VTXProtocol)

#### `bool begin(HardwareSerial* serial, uint8_t txPin = 16, uint8_t rxPin = 17)`
Initialize the VTX connection.

#### `void update()`
Process incoming data and state machine. **Must be called regularly!**

#### `bool isReady()`
Check if VTX is detected and ready.

#### `bool setFrequency(uint16_t freq)`
Set frequency in MHz (5000-5999).

#### `bool setPower(uint16_t power)`
Set power in mW (e.g., 25, 200, 400, 600).

#### `bool setPitMode(bool enable)`
Enable/disable pit mode (low power).

#### `uint16_t getFrequency()`
Get current frequency.

#### `uint16_t getPower()`
Get current power level.

#### `bool getPitMode()`
Get current pit mode status.

### SmartAudio Specific

#### `bool setBandAndChannel(uint8_t band, uint8_t channel)`
Set band (1-5) and channel (1-8).
- Band 1: A (Boscam A)
- Band 2: B (Boscam B)
- Band 3: E (Boscam E)
- Band 4: F (Fatshark/NexWave)
- Band 5: R (Raceband)

#### `uint8_t getVersion()`
Get SmartAudio version (1, 2, or 3).

### TRAMP Specific

#### `uint16_t getTemperature()`
Get VTX temperature in Celsius.

## Protocol Details

### SmartAudio Protocol

- **Baudrate:** 4800-4950 (auto-detected)
- **Packet format:** `[0xAA][0x55][CMD][LEN][DATA...][CRC8]`
- **CRC:** Polynomial 0xD5
- **Versions:** v1, v2, v2.1

### TRAMP Protocol

- **Baudrate:** 9600 (fixed)
- **Packet format:** `[0x0F][CMD][PARAM_LO][PARAM_HI][...][CHECKSUM][0x00]`
- **Packet size:** 16 bytes
- **Commands:** 'r', 'v', 's', 'F', 'P', 'I'

## Frequency Tables

### Raceband (Band 5)
| Channel | Frequency |
|---------|-----------|
| 1 | 5658 MHz |
| 2 | 5695 MHz |
| 3 | 5732 MHz |
| 4 | 5769 MHz |
| 5 | 5806 MHz |
| 6 | 5843 MHz |
| 7 | 5880 MHz |
| 8 | 5917 MHz |

See [frequency tables](docs/FREQUENCIES.md) for all bands.

## Troubleshooting

### VTX not detected
- Check wiring and voltage levels
- Verify UART pins (default GPIO 16/17)
- Ensure VTX is powered
- Call `update()` regularly in loop()

### Commands not working
- VTX might be in race lock mode
- Check protocol compatibility (SmartAudio vs TRAMP)
- Verify baud rate (SmartAudio auto-detects)

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
