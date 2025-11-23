/**
 * SmartAudio Basic Example
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 (TX) to VTX SmartAudio pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 * 
 * Note: TX-only mode - commands sent, no responses expected
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

#define VTX_TX_PIN 16  // ESP32 GPIO pin for VTX control

SmartAudioVTX vtx;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("SmartAudio VTX Control Example");
  Serial.println("================================");
  
  if (vtx.begin(&Serial2, VTX_TX_PIN)) {
    Serial.println("VTX initialized successfully");
    
    // Configure VTX
    Serial.println("\nConfiguring VTX...");
    
    vtx.setBandAndChannel(5, 1);
    Serial.println("Set to Raceband CH1 (5658 MHz)");
    delay(300);
    
    vtx.setPower(200);
    Serial.println("Set power to 200mW");
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
