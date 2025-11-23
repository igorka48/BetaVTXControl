/**
 * @file SmartAudio.h
 * @brief SmartAudio VTX protocol implementation
 * 
 * Based on Betaflight vtx_smartaudio.c
 * Supports SmartAudio v1, v2, and v2.1
 */

#ifndef SMARTAUDIO_H
#define SMARTAUDIO_H

#include "VTXProtocol.h"

// Fixed baud rate as per Betaflight/esp-fc (no auto-baud in TX-only mode)
#define VTX_SMARTAUDIO_BAUD_4800    4800
#define VTX_TX_BUFFER_SIZE          255

// Band and channel constants for setBandAndChannel()
#define VTX_MIN_BAND        1
#define VTX_MAX_BAND        5
#define VTX_MIN_CHANNEL     1
#define VTX_MAX_CHANNEL     8

#define SA_MAX_PACKET_LEN   21

#define SA_CMD_NONE         0x00
#define SA_CMD_GET_SETTINGS 0x01
#define SA_CMD_SET_POWER    0x02
#define SA_CMD_SET_CHAN     0x03
#define SA_CMD_SET_FREQ     0x04
#define SA_CMD_SET_MODE     0x05
#define SA_CMD_GET_SETTINGS_V2  0x09
#define SA_CMD_GET_SETTINGS_V21 0x11

#define SA_PREAMBLE_1       0xAA
#define SA_PREAMBLE_2       0x55

#define SA_MODE_GET_PITMODE     0x02
#define SA_MODE_GET_IN_RANGE    0x04
#define SA_MODE_GET_OUT_RANGE   0x08
#define SA_MODE_GET_UNLOCK      0x10
#define SA_MODE_GET_FREQ_MODE   0x01

#define SA_MODE_SET_IN_RANGE    0x01
#define SA_MODE_SET_OUT_RANGE   0x02
#define SA_MODE_CLR_PITMODE     0x04
#define SA_MODE_SET_UNLOCK      0x08

#define SA_CMD_TIMEOUT          120
#define SA_POLLING_INTERVAL     150
#define SA_POLLING_WINDOW       1000

#define SA_QUEUE_SIZE           4
#define SA_MAX_CMD_BUF_SIZE     32

class SmartAudioVTX : public VTXProtocol {
public:
    SmartAudioVTX();
    ~SmartAudioVTX();
    
    bool begin(HardwareSerial* serial, uint8_t txPin) override;
    void update() override;
    bool isReady() override;
    bool setFrequency(uint16_t freq) override;
    bool setPower(uint16_t power) override;
    bool setPitMode(bool enable) override;
    
    /**
     * @brief Set band and channel
     * @param band Band number (1-5)
     * @param channel Channel number (1-8)
     * @return true if command sent successfully
     */
    bool setBandAndChannel(uint8_t band, uint8_t channel);
    
    /**
     * @brief Set power by raw index (0-4)
     * Use this if setPower(mW) doesn't work for your VTX
     * @param index Power index (device-specific, typically 0-4)
     * @return true if command sent successfully
     */
    bool setPowerByIndex(uint8_t index);
    
    struct Statistics {
        uint16_t packetsSent;
        uint16_t packetsReceived;
        uint16_t crcErrors;
        uint16_t badLength;
        uint16_t badPreamble;
    };
    
    Statistics getStatistics() { return _stats; }

private:
    enum ReceiveState {
        WAIT_PREAMBLE_1,
        WAIT_PREAMBLE_2,
        WAIT_COMMAND,
        WAIT_LENGTH,
        WAIT_DATA,
        WAIT_CRC
    };
    
    enum InitPhase {
        INIT_START,
        INIT_WAIT_SETTINGS,
        INIT_WAIT_PITFREQ,
        INIT_DONE
    };
    
    uint8_t _saVersion = 0;
    uint8_t _saChan = 0;
    uint8_t _saChannel = 0;  // Channel from settings response
    uint8_t _saPower = 0;
    uint8_t _saMode = 0;
    uint16_t _saFreq = 0;      // Current frequency from VTX
    uint16_t _saPitFreq = 0;   // Pit mode frequency
    uint16_t _currentBaud = VTX_SMARTAUDIO_BAUD_4800;
    
    ReceiveState _rxState = WAIT_PREAMBLE_1;
    InitPhase _initPhase = INIT_START;
    uint8_t _rxBuffer[SA_MAX_PACKET_LEN];
    uint8_t _rxPos = 0;
    uint8_t _rxLength = 0;
    uint8_t _rxCommand = 0;
    
    struct Command {
        uint8_t buffer[SA_MAX_CMD_BUF_SIZE];
        uint8_t length;
    };
    Command _cmdQueue[SA_QUEUE_SIZE];
    uint8_t _queueHead = 0;
    uint8_t _queueTail = 0;
    
    unsigned long _lastTransmission = 0;
    unsigned long _lastCommand = 0;
    uint8_t _outstandingCmd = SA_CMD_NONE;
    
    Statistics _stats = {0, 0, 0, 0, 0};
    
    uint8_t powerMwToIndex(uint16_t powerMw);
    uint8_t calculateCRC8(const uint8_t* data, uint8_t len);
    void sendFrame(uint8_t* buf, uint8_t len);
    void queueCommand(uint8_t* buf, uint8_t len);
    void sendQueue();
    void processResponse(uint8_t* buf, uint8_t len);
    void receiveChar(uint8_t c);
    void getSettings();
    void setMode(uint8_t mode);
};

#endif // SMARTAUDIO_H
