/**
 * Debug Example
 * 
 * This example demonstrates how to enable debug output
 * to see raw HEX commands sent to the VTX.
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 (TX) to VTX control pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 * 
 * Debug output will be printed to Serial monitor (USB).
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

#define VTX_TX_PIN 16  // ESP32 GPIO pin for VTX control

BetaVTXControl vtx(VTX_PROTOCOL_SMARTAUDIO); // or VTX_PROTOCOL_TRAMP

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("\n=== VTX Control Debug Example ===");
  Serial.println("Watch raw HEX commands below:\n");
  
  // Enable debug by passing Serial as third parameter
  if (vtx.begin(&Serial2, VTX_TX_PIN, &Serial)) {
    Serial.println("\nSending commands...\n");
    
    // Set frequency
    Serial.println("Command: setFrequency(5732)");
    vtx.setFrequency(5732);
    delay(300);
    
    // Set power
    Serial.println("\nCommand: setPower(200)");
    vtx.setPower(200);
    delay(300);
    
    // Disable pit mode
    Serial.println("\nCommand: setPitMode(false)");
    vtx.setPitMode(false);
    delay(300);
    
    Serial.println("\n=== All commands sent! ===");
  } else {
    Serial.println("Failed to initialize VTX");
  }
}

void loop() {
  // TX-only mode: no need to call update()
  delay(100);
}
