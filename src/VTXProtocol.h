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
     * @return true if initialization successful
     */
    virtual bool begin(HardwareSerial* serial, uint8_t txPin) = 0;
    
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
    uint8_t _txPin = 0;
    
    bool _isReady = false;
};

#endif // VTXPROTOCOL_H
