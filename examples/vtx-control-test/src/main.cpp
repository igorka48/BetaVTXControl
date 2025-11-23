/**
 * VTX Control Test - TX-only with dummy byte
 * 
 * Commands:
 *   1 - Send SmartAudio GET_SETTINGS
 *   2 - Send SmartAudio SET_FREQ 5740 MHz
 *   3 - Send SmartAudio SET_POWER 200 mW
 *   4 - Send TRAMP GET_CONFIG
 *   5 - Send TRAMP SET_FREQ 5740 MHz
 *   h - Show help
 * 
 * Hardware:
 *   - ESP32 GPIO 16 & 17 connected together to VTX control pin
 *   - Common ground between ESP32 and VTX
 */

#include <Arduino.h>

#define TX_PIN 16
#define RX_PIN 17

void printHelp() {
  Serial.println("\n=== VTX Control Test (TX-only) ===");
  Serial.println("SmartAudio Commands:");
  Serial.println("  1 - Send GET_SETTINGS");
  Serial.println("  2 - Send SET_FREQ 5740 MHz");
  Serial.println("  3 - Send SET_POWER 25mW (min)");
  Serial.println("  4 - Send SET_POWER 200mW (mid)");
  Serial.println("  5 - Send SET_POWER 800mW (max)");
  Serial.println("  6 - Send PIT MODE ON");
  Serial.println("  7 - Send PIT MODE OFF");
  Serial.println("\nTRAMP Commands:");
  Serial.println("  8 - Send GET_CONFIG");
  Serial.println("  9 - Send SET_FREQ 5740 MHz");
  Serial.println("\nOther:");
  Serial.println("  h - Show this help");
  Serial.println("===================================\n");
}

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a) {
  crc ^= a;
  for (int i = 0; i < 8; i++) {
    if (crc & 0x80) {
      crc = (crc << 1) ^ 0xD5;
    } else {
      crc = crc << 1;
    }
  }
  return crc;
}

void sendSmartAudio(uint8_t* data, int len) {
  Serial.print("TX (SmartAudio 4800 baud): ");
  for (int i = 0; i < len; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Send dummy byte for UART stabilization (as per esp-fc)
  static const uint8_t dummyByte = 0x00;
  Serial2.write(&dummyByte, 1);
  
  Serial2.write(data, len);
  Serial2.flush();
  
  Serial.println("✓ Sent\n");
}

void sendTramp(uint8_t* data, int len) {
  Serial.print("TX (TRAMP 9600 baud): ");
  for (int i = 0; i < len; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Send dummy byte for UART stabilization (as per esp-fc)
  static const uint8_t dummyByte = 0x00;
  Serial2.write(&dummyByte, 1);
  
  Serial2.write(data, len);
  Serial2.flush();
  
  Serial.println("✓ Sent\n");
}

void sendSmartAudioGetSettings() {
  Serial.println("[1] SmartAudio GET_SETTINGS");
  uint8_t cmd[] = {0xAA, 0x55, 0x01, 0x00, 0x9F};
  sendSmartAudio(cmd, 5);
}

void sendSmartAudioSetFreq5740() {
  Serial.println("[2] SmartAudio SET_FREQ 5740 MHz");
  // AA 55 07 02 <freq_hi> <freq_lo> <crc>
  uint8_t cmd[] = {0xAA, 0x55, 0x07, 0x02, 0x16, 0x6C, 0x00};
  
  // Calculate CRC
  uint8_t crc = 0;
  for (int i = 0; i < 6; i++) {
    crc = crc8_dvb_s2(crc, cmd[i]);
  }
  cmd[6] = crc;
  
  sendSmartAudio(cmd, 7);
}

void sendSmartAudioSetPower200() {
  Serial.println("[4] SmartAudio SET_POWER 200 mW (level 2)");
  // AA 55 05 01 <power_level> <crc>
  uint8_t cmd[] = {0xAA, 0x55, 0x05, 0x01, 0x02, 0x00};
  
  // Calculate CRC
  uint8_t crc = 0;
  for (int i = 0; i < 5; i++) {
    crc = crc8_dvb_s2(crc, cmd[i]);
  }
  cmd[5] = crc;
  
  sendSmartAudio(cmd, 6);
}

void sendSmartAudioSetPower25() {
  Serial.println("[3] SmartAudio SET_POWER 25 mW (level 0 - min)");
  // AA 55 05 01 <power_level> <crc>
  uint8_t cmd[] = {0xAA, 0x55, 0x05, 0x01, 0x00, 0x00};
  
  // Calculate CRC
  uint8_t crc = 0;
  for (int i = 0; i < 5; i++) {
    crc = crc8_dvb_s2(crc, cmd[i]);
  }
  cmd[5] = crc;
  
  sendSmartAudio(cmd, 6);
}

void sendSmartAudioSetPower800() {
  Serial.println("[5] SmartAudio SET_POWER 800 mW (level 3 - max)");
  // AA 55 05 01 <power_level> <crc>
  uint8_t cmd[] = {0xAA, 0x55, 0x05, 0x01, 0x03, 0x00};
  
  // Calculate CRC
  uint8_t crc = 0;
  for (int i = 0; i < 5; i++) {
    crc = crc8_dvb_s2(crc, cmd[i]);
  }
  cmd[5] = crc;
  
  sendSmartAudio(cmd, 6);
}

void sendSmartAudioPitModeOn() {
  Serial.println("[6] SmartAudio PIT MODE ON");
  // AA 55 0B 01 <mode> <crc>
  // mode: 0x01 = in-range (pit mode)
  uint8_t cmd[] = {0xAA, 0x55, 0x0B, 0x01, 0x01, 0x00};
  
  // Calculate CRC
  uint8_t crc = 0;
  for (int i = 0; i < 5; i++) {
    crc = crc8_dvb_s2(crc, cmd[i]);
  }
  cmd[5] = crc;
  
  sendSmartAudio(cmd, 6);
}

void sendSmartAudioPitModeOff() {
  Serial.println("[7] SmartAudio PIT MODE OFF");
  // AA 55 0B 01 <mode> <crc>
  // mode: 0x04 = clear pit mode
  uint8_t cmd[] = {0xAA, 0x55, 0x0B, 0x01, 0x04, 0x00};
  
  // Calculate CRC
  uint8_t crc = 0;
  for (int i = 0; i < 5; i++) {
    crc = crc8_dvb_s2(crc, cmd[i]);
  }
  cmd[5] = crc;
  
  sendSmartAudio(cmd, 6);
}

uint8_t trampChecksum(uint8_t* buf) {
  uint8_t cksum = 0;
  for (int i = 1; i < 14; i++) {
    cksum += buf[i];
  }
  return cksum;
}

void sendTrampGetConfig() {
  Serial.println("[8] TRAMP GET_CONFIG");
  uint8_t cmd[16];
  memset(cmd, 0, 16);
  cmd[0] = 0x0F;  // header
  cmd[1] = 0x76;  // 'v' command
  cmd[14] = trampChecksum(cmd);
  cmd[15] = 0x00;
  
  sendTramp(cmd, 16);
}

void sendTrampSetFreq5740() {
  Serial.println("[9] TRAMP SET_FREQ 5740 MHz");
  uint8_t cmd[16];
  memset(cmd, 0, 16);
  cmd[0] = 0x0F;  // header
  cmd[1] = 0x46;  // 'F' command
  cmd[2] = 0x6C;  // 5740 low byte
  cmd[3] = 0x16;  // 5740 high byte
  cmd[14] = trampChecksum(cmd);
  cmd[15] = 0x00;
  
  sendTramp(cmd, 16);
}

void processCommand(String cmd) {
  cmd.trim();
  
  if (cmd.length() == 0) return;
  
  char command = cmd.charAt(0);
  
  switch (command) {
    case 'h':
    case 'H':
      printHelp();
      break;
      
    case '1':
      sendSmartAudioGetSettings();
      break;
      
    case '2':
      sendSmartAudioSetFreq5740();
      break;
      
    case '3':
      sendSmartAudioSetPower25();
      break;
      
    case '4':
      sendSmartAudioSetPower200();
      break;
      
    case '5':
      sendSmartAudioSetPower800();
      break;
      
    case '6':
      sendSmartAudioPitModeOn();
      break;
      
    case '7':
      sendSmartAudioPitModeOff();
      break;
      
    case '8':
      sendTrampGetConfig();
      break;
      
    case '9':
      sendTrampSetFreq5740();
      break;
      
    default:
      Serial.print("✗ Unknown command: ");
      Serial.println(cmd);
      Serial.println("Type 'h' for help");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  delay(1000);
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════╗");
  Serial.println("║   VTX Control Test (TX-only)      ║");
  Serial.println("╚════════════════════════════════════╝");
  
  Serial.print("TX Pin: GPIO ");
  Serial.print(TX_PIN);
  Serial.println(" & GPIO ");
  Serial.print(RX_PIN);
  Serial.println(" (soldered together)");
  
  Serial.println("\n⚠️  TX-only mode - no responses expected");
  Serial.println("    Watch VTX LED for confirmation\n");
  
  // Initialize serial with both TX and RX pins (standard begin)
  Serial2.setTxBufferSize(255);
  Serial2.begin(4800, SERIAL_8N2, RX_PIN, TX_PIN);
  
  printHelp();
  Serial.print("> ");
}

void loop() {
  static String inputBuffer = "";
  
  // Process serial commands
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        Serial.println();
        processCommand(inputBuffer);
        inputBuffer = "";
        Serial.print("> ");
      }
    } else if (c == '\b' || c == 127) {
      // Backspace
      if (inputBuffer.length() > 0) {
        inputBuffer.remove(inputBuffer.length() - 1);
        Serial.print("\b \b");
      }
    } else if (c >= 32 && c < 127) {
      // Printable character
      inputBuffer += c;
      Serial.print(c);
    }
  }
  
  delay(10);
}
