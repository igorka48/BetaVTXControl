/**
 * @file TRAMP.h
 * @brief TRAMP VTX protocol implementation
 * 
 * Based on Betaflight vtx_tramp.c
 */

#ifndef TRAMP_H
#define TRAMP_H

#include "VTXProtocol.h"

// Fixed baud rate as per TRAMP protocol
#define TRAMP_BAUD              9600
#define VTX_TX_BUFFER_SIZE      255

#define TRAMP_PACKET_SIZE       16
#define TRAMP_HEADER            0x0F

#define TRAMP_CMD_RESET         'r'
#define TRAMP_CMD_STATUS        'v'
#define TRAMP_CMD_TEMP          's'
#define TRAMP_CMD_SET_FREQ      'F'
#define TRAMP_CMD_SET_POWER     'P'
#define TRAMP_CMD_SET_ACTIVE    'I'

#define TRAMP_CONTROL_RACE_LOCK 0x01

#define TRAMP_MIN_REQUEST_PERIOD    200000
#define TRAMP_STATUS_REQUEST_PERIOD 1000000

#define TRAMP_MAX_RETRIES       20

class TrampVTX : public VTXProtocol {
public:
    TrampVTX();
    ~TrampVTX();
    
    bool begin(HardwareSerial* serial, uint8_t txPin = 16, uint8_t rxPin = 17) override;
    void update() override;
    bool isReady() override;
    bool setFrequency(uint16_t freq) override;
    bool setPower(uint16_t power) override;
    bool setPitMode(bool enable) override;

private:
    enum Status {
        STATUS_OFFLINE,
        STATUS_INIT,
        STATUS_ONLINE_MONITOR_FREQPWRPIT,
        STATUS_ONLINE_MONITOR_TEMP,
        STATUS_ONLINE_CONFIG
    };
    
    enum ReceiveState {
        RX_WAIT_LEN,
        RX_WAIT_CODE,
        RX_DATA
    };
    
    uint16_t _confFreq = 0;
    uint16_t _confPower = 0;
    bool _confPitMode = false;
    
    Status _status = STATUS_OFFLINE;
    ReceiveState _rxState = RX_WAIT_LEN;
    
    uint8_t _txBuffer[TRAMP_PACKET_SIZE];
    uint8_t _rxBuffer[TRAMP_PACKET_SIZE];
    uint8_t _rxPos = 0;
    
    unsigned long _lastRequest = 0;
    uint8_t _retryCount = TRAMP_MAX_RETRIES;
    
    uint8_t calculateChecksum(const uint8_t* buf);
    void sendPacket(uint8_t cmd, uint16_t param);
    void sendCommand(uint8_t cmd, uint16_t param);
    void query(uint8_t cmd);
    char receive();
    char handleResponse();
    void resetReceiver();
};

#endif
