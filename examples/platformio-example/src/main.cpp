/**
 * BetaVTXControl - PlatformIO Example (TX-only mode)
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 and 17 soldered together, connected to VTX control pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 * 
 * Protocol Selection:
 * - Change VTX_PROTOCOL_SMARTAUDIO to VTX_PROTOCOL_TRAMP if needed
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

// Select protocol: VTX_PROTOCOL_SMARTAUDIO or VTX_PROTOCOL_TRAMP
BetaVTXControl vtx(VTX_PROTOCOL_SMARTAUDIO);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("BetaVTXControl - PlatformIO Example");
  Serial.println("====================================");
  Serial.println("Protocol: SmartAudio (TX-only)");
  
  if (vtx.begin(&Serial2, VTX_DEFAULT_TX_PIN, VTX_DEFAULT_RX_PIN)) {
    Serial.println("✓ VTX initialized");
    
    // Configure VTX (commands are sent but responses not expected in TX-only mode)
    Serial.println("\nConfiguring VTX...");
    vtx.setFrequency(5732);  // R1 (Raceband 1)
    delay(300);
    
    vtx.setPower(200);       // 200mW
    delay(300);
    
    vtx.setPitMode(false);   // Pit mode OFF
    delay(300);
    
    Serial.println("✓ VTX configuration sent");
    Serial.println("\nNote: In TX-only mode, VTX responses are not monitored.");
    Serial.println("Check VTX LED for confirmation of settings.");
  } else {
    Serial.println("✗ Failed to initialize VTX");
    while (1) delay(100);
  }
}

void loop() {
  vtx.update();
  
  // Periodically send commands to maintain VTX state
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 5000) {
    Serial.println("Sending periodic update...");
    vtx.setFrequency(5732);
    vtx.setPower(200);
    lastUpdate = millis();
  }
  
  delay(10);
}
