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
  
void loop() {
  vtx.update();
  
  if (vtx.isReady()) {
    static bool configured = false;
    if (!configured) {
      Serial.println("\nVTX is ready!");
      Serial.println("Configuring VTX...");
      
      vtx.setBandAndChannel(5, 1);
      Serial.println("Set to Raceband CH1 (5658 MHz)");
      delay(300);
      
      vtx.setPower(200);
      Serial.println("Set power to 200mW");
      delay(300);
      
      vtx.setPitMode(false);
      Serial.println("Pit mode disabled");
      delay(300);
      
      Serial.println("VTX configured!\n");
      configured = true;
    }
    
    // Periodically send commands to maintain state
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 5000) {
      Serial.println("--- Sending periodic commands ---");
      vtx.setBandAndChannel(5, 1);
      delay(300);
      vtx.setPower(200);
      delay(300);
      
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
  
  delay(10);
}

}
