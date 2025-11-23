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

#define VTX_MIN_FREQUENCY_MHZ 5000
#define VTX_MAX_FREQUENCY_MHZ 5999

#define VTX_BAND_A 1  // Boscam A
#define VTX_BAND_B 2  // Boscam B
#define VTX_BAND_E 3  // Boscam E
#define VTX_BAND_F 4  // Fatshark/NexWave
#define VTX_BAND_R 5  // Raceband

#define VTX_MIN_BAND 1
#define VTX_MAX_BAND 5
#define VTX_MIN_CHANNEL 1
#define VTX_MAX_CHANNEL 8

#define VTX_POWER_OFF   0
#define VTX_POWER_25    25
#define VTX_POWER_100   100
#define VTX_POWER_200   200
#define VTX_POWER_400   400
#define VTX_POWER_600   600
#define VTX_POWER_800   800

class VTXProtocol {
public:
    virtual ~VTXProtocol() {}
    
    /**
     * @brief Initialize VTX communication
     * @param serial Pointer to HardwareSerial port
     * @param txPin TX pin number
     * @param rxPin RX pin number
     * @return true if initialization successful
     */
    virtual bool begin(HardwareSerial* serial, uint8_t txPin = 16, uint8_t rxPin = 17) = 0;
    
    virtual void update() = 0;
    virtual bool isReady() = 0;
    
    /**
     * @param freq Frequency in MHz (5000-5999)
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
    uint8_t _txPin = 16;
    uint8_t _rxPin = 17;
    
    bool _isReady = false;
    
    bool validateFrequency(uint16_t freq) {
        return (freq >= VTX_MIN_FREQUENCY_MHZ && freq <= VTX_MAX_FREQUENCY_MHZ);
    }
    
    /**
     * @brief Convert band and channel to frequency
     * @param band Band number (1-5)
     * @param channel Channel number (1-8)
     * @return Frequency in MHz
     */
    uint16_t bandChannelToFreq(uint8_t band, uint8_t channel);
};

#endif // VTXPROTOCOL_H
