/**
 * @file BetaVTXControl.h
 * @brief Main header file for BetaVTXControl library
 * 
 * ESP32 Arduino library for controlling VTX modules via SmartAudio and TRAMP protocols
 * Based on Betaflight implementation
 * 
 * @author BetaVTXControl Contributors
 * @license MIT
 */

#ifndef BETAVTXCONTROL_H
#define BETAVTXCONTROL_H

#include "VTXProtocol.h"
#include "SmartAudio.h"
#include "TRAMP.h"

#define BETAVTXCONTROL_VERSION "1.0.0"

#define VTX_DEFAULT_TX_PIN 16
#define VTX_DEFAULT_RX_PIN 17

enum VTXProtocolType {
    VTX_PROTOCOL_SMARTAUDIO,
    VTX_PROTOCOL_TRAMP
};

class BetaVTXControl {
public:
    /**
     * @param protocolType Protocol to use (VTX_PROTOCOL_SMARTAUDIO or VTX_PROTOCOL_TRAMP)
     */
    BetaVTXControl(VTXProtocolType protocolType);
    ~BetaVTXControl();
    
    /**
     * @brief Initialize VTX communication
     * @param serial Pointer to HardwareSerial port
     * @param txPin TX pin number
     * @param rxPin RX pin number
     * @return true if initialization successful
     */
    bool begin(HardwareSerial* serial, uint8_t txPin = VTX_DEFAULT_TX_PIN, uint8_t rxPin = VTX_DEFAULT_RX_PIN);
    
    void update();
    bool isReady();
    
    /**
     * @param freq Frequency in MHz (5000-5999)
     */
    bool setFrequency(uint16_t freq);
    
    /**
     * @param power Power in mW
     */
    bool setPower(uint16_t power);
    
    /**
     * @param enable true to enable pit mode
     */
    bool setPitMode(bool enable);
    
    /**
     * @return Protocol type
     */
    VTXProtocolType getProtocolType() { return _protocolType; }
    
    static const char* getVersion() { return BETAVTXCONTROL_VERSION; }

private:
    VTXProtocolType _protocolType;
    VTXProtocol* _vtx = nullptr;
};

#endif
