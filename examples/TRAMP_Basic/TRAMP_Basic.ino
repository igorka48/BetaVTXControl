/**
 * TRAMP Basic Example
 * 
 * Hardware Setup:
 * - ESP32 GPIO 16 (TX2) to VTX TRAMP pin
 * - Common ground between ESP32 and VTX
 * - VTX powered separately
 * 
 * Note: TRAMP uses half-duplex UART at 9600 baud
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

TrampVTX vtx;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("TRAMP VTX Control Example");
  Serial.println("=========================");
  
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
    static bool configured = false;
    if (!configured) {
      Serial.println("\nVTX is ready!");
      Serial.println("Configuring VTX...");
      
      vtx.setFrequency(5740);
      Serial.println("Set frequency to 5740 MHz");
      delay(300);
      
      vtx.setPower(400);
      Serial.println("Set power to 400mW");
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
      vtx.setFrequency(5740);
      delay(300);
      vtx.setPower(400);
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

