<!-- BetaVTXControl Project Instructions -->

## Project Type
Arduino library for ESP32 to control VTX modules via SmartAudio and TRAMP protocols.
Based on Betaflight implementation.

## Project Structure
- src/ - Source code (headers and implementation)
- examples/ - Arduino sketches demonstrating usage
- library.json - PlatformIO metadata
- library.properties - Arduino IDE metadata
- README.md - Documentation
- LICENSE - MIT License
- keywords.txt - Arduino IDE keywords

## Development Guidelines
- Non-blocking operations using state machines
- Support for HardwareSerial UART communication
- CRC8 validation for SmartAudio protocol
- Checksum validation for TRAMP protocol
- Thread-safe for FreeRTOS compatibility
