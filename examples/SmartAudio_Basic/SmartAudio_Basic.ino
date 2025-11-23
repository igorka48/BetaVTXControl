/**
 * SmartAudio Basic Example
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 (TX2) to VTX SmartAudio pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 * 
 * Note: SmartAudio uses half-duplex UART (single wire for TX/RX)
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

SmartAudioVTX vtx;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("SmartAudio VTX Control Example");
  Serial.println("================================");
  
  if (vtx.begin(&Serial2, 16, 17)) {
    Serial.println("✓ VTX initialized successfully");
  } else {
    Serial.println("✗ Failed to initialize VTX");
    while (1) delay(100);
  }
  
  Serial.println("Waiting for VTX to respond...");
}

void loop() {
  vtx.update();
  
  if (vtx.isReady()) {
    static bool infoDisplayed = false;
    if (!infoDisplayed) {
      Serial.println("\n✓ VTX is ready!");
      Serial.print("Version: SmartAudio v");
      Serial.println(vtx.getVersion());
      Serial.print("Current Frequency: ");
      Serial.print(vtx.getFrequency());
      Serial.println(" MHz");
      Serial.print("Current Power: ");
      Serial.print(vtx.getPower());
      Serial.println(" mW");
      Serial.print("Pit Mode: ");
      Serial.println(vtx.getPitMode() ? "ON" : "OFF");
      
      infoDisplayed = true;
      
      Serial.println("\nConfiguring VTX...");
      
      vtx.setBandAndChannel(5, 1);
      Serial.println("✓ Set to Raceband CH1 (5658 MHz)");
      
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
      
      // Display statistics
      auto stats = vtx.getStatistics();
      Serial.print("Packets - Sent: ");
      Serial.print(stats.packetsSent);
      Serial.print(", Received: ");
      Serial.print(stats.packetsReceived);
      Serial.print(", CRC Errors: ");
      Serial.println(stats.crcErrors);
      Serial.println();
      
      lastUpdate = millis();
    }
  } else {
    // Still waiting for VTX
    static unsigned long lastDot = 0;
    if (millis() - lastDot >= 500) {
      Serial.print(".");
      lastDot = millis();
    }
  }
  
  delay(10); // Small delay to prevent tight loop
}
