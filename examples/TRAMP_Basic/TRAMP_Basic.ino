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
    static bool infoDisplayed = false;
    if (!infoDisplayed) {
      Serial.println("\n✓ VTX is ready!");
      Serial.print("Min Frequency: ");
      Serial.print(vtx.getMinFrequency());
      Serial.println(" MHz");
      Serial.print("Max Frequency: ");
      Serial.print(vtx.getMaxFrequency());
      Serial.println(" MHz");
      Serial.print("Max Power: ");
      Serial.print(vtx.getMaxPower());
      Serial.println(" mW");
      
      Serial.print("\nCurrent Frequency: ");
      Serial.print(vtx.getFrequency());
      Serial.println(" MHz");
      Serial.print("Current Power: ");
      Serial.print(vtx.getPower());
      Serial.println(" mW");
      Serial.print("Actual Power: ");
      Serial.print(vtx.getActualPower());
      Serial.println(" mW");
      Serial.print("Pit Mode: ");
      Serial.println(vtx.getPitMode() ? "ON" : "OFF");
      Serial.print("Race Lock: ");
      Serial.println(vtx.isRaceLocked() ? "YES" : "NO");
      
      infoDisplayed = true;
      
      if (!vtx.isRaceLocked()) {
        Serial.println("\nConfiguring VTX...");
        
        vtx.setFrequency(5740);
        Serial.println("✓ Set frequency to 5740 MHz");
        
        // Set power to 400mW
        vtx.setPower(400);
        Serial.println("✓ Set power to 400mW");
        
        // Disable pit mode
        vtx.setPitMode(false);
        Serial.println("✓ Pit mode disabled");
        
        Serial.println("\nVTX configured! Monitoring status...\n");
      } else {
        Serial.println("\n⚠ VTX is race locked - cannot change settings");
      }
    }
    
    // Display status every 5 seconds
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 5000) {
      Serial.println("--- Status ---");
      Serial.print("Freq: ");
      Serial.print(vtx.getFrequency());
      Serial.print(" MHz | Power: ");
      Serial.print(vtx.getPower());
      Serial.print(" mW (actual: ");
      Serial.print(vtx.getActualPower());
      Serial.print(" mW)");
      Serial.println();
      
      Serial.print("Pit: ");
      Serial.print(vtx.getPitMode() ? "ON" : "OFF");
      Serial.print(" | Temperature: ");
      Serial.print(vtx.getTemperature());
      Serial.println("°C");
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
