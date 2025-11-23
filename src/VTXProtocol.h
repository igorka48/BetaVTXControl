/**
 * @file VTXProtocol.h
 * @brief Abstract base class for VTX protocol implementations
 * 
 * Based on Betaflight VTX control implementation
 * https://github.com/betaflight/betaflight
 */

#ifndef VTXPROTOCOL_H
#define VTXPROTOCOL_H

#include <Arduino.h>
#include <HardwareSerial.h>

class VTXProtocol {
public:
    virtual ~VTXProtocol() {}
    
    /**
     * @brief Initialize VTX communication (TX-only mode)
     * @param serial Pointer to HardwareSerial port
     * @param txPin TX pin number
     * @param debugSerial Optional debug serial port for raw command output (default: nullptr)
     * @return true if initialization successful
     */
    virtual bool begin(HardwareSerial* serial, uint8_t txPin, HardwareSerial* debugSerial = nullptr) = 0;
    
    virtual void update() = 0;
    virtual bool isReady() = 0;
    
    /**
     * @param freq Frequency in MHz
     */
    virtual bool setFrequency(uint16_t freq) = 0;
    
    /**
     * @param power Power level in mW
     */
    virtual bool setPower(uint16_t power) = 0;
    
    /**
     * @param enable true to enable pit mode
     */
    virtual bool setPitMode(bool enable) = 0;

protected:
    HardwareSerial* _serial = nullptr;
    HardwareSerial* _debugSerial = nullptr;
    uint8_t _txPin = 0;
    
    bool _isReady = false;
    
    /**
     * @brief Print raw command in HEX format to debug serial
     * @param buf Command buffer
     * @param len Buffer length
     * @param label Optional label (e.g., "SmartAudio", "TRAMP")
     */
    void debugPrintHex(const uint8_t* buf, uint8_t len, const char* label = "TX") {
        if (!_debugSerial) return;
        
        _debugSerial->print("[");
        _debugSerial->print(label);
        _debugSerial->print("] ");
        for (uint8_t i = 0; i < len; i++) {
            if (buf[i] < 0x10) _debugSerial->print("0");
            _debugSerial->print(buf[i], HEX);
            if (i < len - 1) _debugSerial->print(" ");
        }
        _debugSerial->println();
    }
};

#endif // VTXPROTOCOL_H
