/**
 * VTX Control Interactive Test
 * 
 * Commands:
 *   1 - Set Raceband CH1 (5658 MHz)
 *   2 - Set Raceband CH3 (5732 MHz)
 *   3 - Set FatShark CH1 (5740 MHz)
 *   4 - Set Power 25mW
 *   5 - Set Power 200mW
 *   6 - Set Power 400mW
 *   7 - Set Power 800mW
 *   8 - Pit Mode ON
 *   9 - Pit Mode OFF
 *   s - Switch protocol (SmartAudio/TRAMP)
 *   h - Show help
 * 
 * Hardware:
 *   - ESP32 GPIO 16 (TX) to VTX control pin
 *   - Common ground between ESP32 and VTX
 */

#include <Arduino.h>
#include <BetaVTXControl.h>

#define VTX_TX_PIN 16

// Start with SmartAudio
BetaVTXControl* vtx = nullptr;
VTXProtocolType currentProtocol = VTX_PROTOCOL_SMARTAUDIO;

void printHelp() {
  Serial.println("\n=== VTX Control Interactive Test ===");
  Serial.print("Current Protocol: ");
  Serial.println(currentProtocol == VTX_PROTOCOL_SMARTAUDIO ? "SmartAudio" : "TRAMP");
  Serial.println("\nFrequency Commands:");
  Serial.println("  1 - Set Raceband CH1 (5658 MHz)");
  Serial.println("  2 - Set Raceband CH3 (5732 MHz)");
  Serial.println("  3 - Set FatShark CH1 (5740 MHz)");
  Serial.println("\nPower Commands:");
  Serial.println("  4 - Set Power 25mW");
  Serial.println("  5 - Set Power 200mW");
  Serial.println("  6 - Set Power 400mW");
  Serial.println("  7 - Set Power 800mW");
  Serial.println("\nPit Mode:");
  Serial.println("  8 - Pit Mode ON");
  Serial.println("  9 - Pit Mode OFF");
  Serial.println("\nOther:");
  Serial.println("  s - Switch protocol");
  Serial.println("  h - Show this help");
  Serial.println("====================================\n");
}

void initVTX() {
  if (vtx) {
    delete vtx;
  }
  
  vtx = new BetaVTXControl(currentProtocol);
  
  if (vtx->begin(&Serial2, VTX_TX_PIN)) {
    Serial.print("VTX initialized (");
    Serial.print(currentProtocol == VTX_PROTOCOL_SMARTAUDIO ? "SmartAudio" : "TRAMP");
    Serial.println(")");
  } else {
    Serial.println("VTX initialization failed!");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("\n\n=== VTX Control Interactive Test ===");
  Serial.println("TX-only mode - Commands sent, no responses expected");
  
  initVTX();
  printHelp();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case '1':
        Serial.println("Setting Raceband CH1 (5658 MHz)...");
        vtx->setFrequency(5658);
        delay(300);
        Serial.println("Done");
        break;
        
      case '2':
        Serial.println("Setting Raceband CH3 (5732 MHz)...");
        vtx->setFrequency(5732);
        delay(300);
        Serial.println("Done");
        break;
        
      case '3':
        Serial.println("Setting FatShark CH1 (5740 MHz)...");
        vtx->setFrequency(5740);
        delay(300);
        Serial.println("Done");
        break;
        
      case '4':
        Serial.println("Setting Power 25mW...");
        vtx->setPower(25);
        delay(300);
        Serial.println("Done");
        break;
        
      case '5':
        Serial.println("Setting Power 200mW...");
        vtx->setPower(200);
        delay(300);
        Serial.println("Done");
        break;
        
      case '6':
        Serial.println("Setting Power 400mW...");
        vtx->setPower(400);
        delay(300);
        Serial.println("Done");
        break;
        
      case '7':
        Serial.println("Setting Power 800mW...");
        vtx->setPower(800);
        delay(300);
        Serial.println("Done");
        break;
        
      case '8':
        Serial.println("Enabling Pit Mode...");
        vtx->setPitMode(true);
        delay(300);
        Serial.println("Done");
        break;
        
      case '9':
        Serial.println("Disabling Pit Mode...");
        vtx->setPitMode(false);
        delay(300);
        Serial.println("Done");
        break;
        
      case 's':
      case 'S':
        currentProtocol = (currentProtocol == VTX_PROTOCOL_SMARTAUDIO) 
                          ? VTX_PROTOCOL_TRAMP 
                          : VTX_PROTOCOL_SMARTAUDIO;
        Serial.print("Switching to ");
        Serial.println(currentProtocol == VTX_PROTOCOL_SMARTAUDIO ? "SmartAudio" : "TRAMP");
        initVTX();
        break;
        
      case 'h':
      case 'H':
        printHelp();
        break;
        
      case '\n':
      case '\r':
        break;
        
      default:
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        Serial.println("Press 'h' for help");
        break;
    }
  }
  
  delay(10);
}
