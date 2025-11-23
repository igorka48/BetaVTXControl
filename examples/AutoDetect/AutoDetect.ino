/**
 * Auto-Detect Example
 * 
 * Automatic VTX protocol detection (SmartAudio or TRAMP).
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 (TX2) to VTX control pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

BetaVTXControl vtx(VTX_PROTOCOL_AUTO);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("BetaVTXControl - Auto-Detection Example");
  Serial.println("========================================");
  
  // Initialize VTX on Serial2 (GPIO 16/17)
  if (vtx.begin(&Serial2, 16, 17)) {
    Serial.println("✓ VTX initialized successfully");
    Serial.println("Detecting protocol...");
  } else {
    Serial.println("✗ Failed to initialize VTX");
    while (1) delay(100);
  }
}

void loop() {
  // IMPORTANT: Must call update() regularly!
  vtx.update();
  
  // Check if VTX is ready
  if (vtx.isReady()) {
    // Display detected protocol once
    static bool protocolDisplayed = false;
    if (!protocolDisplayed) {
      Serial.println("\n✓ VTX detected!");
      Serial.print("Protocol: ");
      
      switch (vtx.getProtocolType()) {
        case VTX_PROTOCOL_SMARTAUDIO:
          Serial.println("SmartAudio");
          break;
        case VTX_PROTOCOL_TRAMP:
          Serial.println("TRAMP");
          break;
        default:
          Serial.println("Unknown");
          break;
      }
      
      Serial.print("Current Frequency: ");
      Serial.print(vtx.getFrequency());
      Serial.println(" MHz");
      Serial.print("Current Power: ");
      Serial.print(vtx.getPower());
      Serial.println(" mW");
      Serial.print("Pit Mode: ");
      Serial.println(vtx.getPitMode() ? "ON" : "OFF");
      
      protocolDisplayed = true;
      
      // Example: Configure VTX
      Serial.println("\nConfiguring VTX...");
      
      // Set frequency to Raceband Channel 3 (5732 MHz)
      vtx.setFrequency(5732);
      Serial.println("✓ Set frequency to 5732 MHz");
      
      vtx.setPower(200);
      Serial.println("✓ Set power to 200mW");
      
      vtx.setPitMode(false);
      Serial.println("✓ Pit mode disabled");
      
      Serial.println("\nVTX configured! Monitoring status...\n");
    }
    
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 5000) {
      Serial.println("--- Status ---");
      Serial.print("Freq: ");
      Serial.print(vtx.getFrequency());
      Serial.print(" MHz | Power: ");
      Serial.print(vtx.getPower());
      Serial.print(" mW | Pit: ");
      Serial.println(vtx.getPitMode() ? "ON" : "OFF");
      Serial.println();
      
      lastUpdate = millis();
    }
  } else {
    static unsigned long lastDot = 0;
    if (millis() - lastDot >= 500) {
      Serial.print(".");
      lastDot = millis();
    }
  }
  
  delay(10);
}
