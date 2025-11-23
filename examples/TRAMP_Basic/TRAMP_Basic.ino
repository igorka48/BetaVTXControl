/**
 * TRAMP Basic Example
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 (TX) to VTX TRAMP pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 * 
 * Note: TX-only mode - commands sent, no responses expected
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

TrampVTX vtx;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("TRAMP VTX Control Example");
  Serial.println("=========================");
  
  if (vtx.begin(&Serial2, 16)) {
    Serial.println("VTX initialized successfully");
    
    // Configure VTX
    Serial.println("\nConfiguring VTX...");
    
    vtx.setFrequency(5740);
    Serial.println("Set frequency to 5740 MHz");
    delay(300);
    
    vtx.setPower(400);
    Serial.println("Set power to 400mW");
    delay(300);
    
    vtx.setPitMode(false);
    Serial.println("Pit mode disabled");
    delay(300);
    
    Serial.println("\nVTX configured!");
  } else {
    Serial.println("Failed to initialize VTX");
    while (1) delay(100);
  }
}

void loop() {
  // TX-only mode: no need to call update()
  // VTX is configured once in setup()
  delay(100);
}
