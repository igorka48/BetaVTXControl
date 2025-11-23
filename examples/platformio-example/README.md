# PlatformIO Example

This example demonstrates how to use BetaVTXControl library with PlatformIO.

## Project Setup

This example is configured to use the library source files directly from the parent directory (`../../src`). This is ideal for local development and testing.

## Installation

### For This Example (Local Development)

The `platformio.ini` is already configured to include the library source files:

```ini
build_flags = 
    -I../../src

build_src_filter = 
    +<*>
    +<../../src/*.cpp>
```

Just build and upload!

### For Your Own Project

**Method 1: From GitHub**

Add to your `platformio.ini`:

```ini
lib_deps = 
    https://github.com/igorka48/BetaVTXControl.git
```

**Method 2: Local Copy**

Copy the entire `BetaVTXControl` folder to your project's `lib/` directory.

## Hardware Setup

- ESP32 GPIO 16 (TX2) → VTX control pin
- ESP32 GND → VTX GND
- VTX powered separately (5V/12V depending on VTX)

## Building and Uploading

```bash
cd examples/platformio-example
pio run -t upload
pio device monitor
```

Or use VS Code PlatformIO extension:
1. Open this folder in VS Code
2. Click "Upload" in PlatformIO toolbar
3. Click "Monitor" to see serial output

## Features Demonstrated

- Auto-detection of SmartAudio or TRAMP protocol
- Setting frequency, power, and pit mode
- Reading VTX status
- Serial monitoring
