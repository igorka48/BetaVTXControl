/**
 * @file SmartAudio.cpp
 * @brief SmartAudio VTX protocol implementation
 * 
 * Based on Betaflight vtx_smartaudio.c
 * Author: jflyper (Betaflight)
 */

#include "SmartAudio.h"

#define CRC8_POLY           0xD5
#define SA_FREQ_GETPIT      0x4000
#define SA_POWER_MASK       0x7F
#define SA_POWER_IN_RANGE   0x80
#define SA_DATA_HEADER_SIZE 4

SmartAudioVTX::SmartAudioVTX() {
    memset(&_stats, 0, sizeof(_stats));
}

SmartAudioVTX::~SmartAudioVTX() {
    if (_serial) {
        _serial->end();
    }
}

bool SmartAudioVTX::begin(HardwareSerial* serial, uint8_t txPin) {
    if (!serial) {
        return false;
    }
    
    _serial = serial;
    _txPin = txPin;
    
    // TX-only mode: configure serial with TX pin only
    // Fixed baud rate 4800 as per Betaflight/esp-fc
    _currentBaud = VTX_SMARTAUDIO_BAUD_4800;
    if (_serial) {
        _serial->end();
    }
    _serial->setTxBufferSize(VTX_TX_BUFFER_SIZE);
    _serial->begin(_currentBaud, SERIAL_8N2, -1, txPin);  // RX=-1 (not used)
    
    _initPhase = INIT_START;
    _isReady = false;
    
    return true;
}

void SmartAudioVTX::update() {
    if (!_serial) {
        return;
    }
    
    while (_serial->available() > 0) {
        uint8_t c = _serial->read();
        receiveChar(c);
    }
    
    // No auto-baud in TX-only mode (fixed 4800 baud)
    
    switch (_initPhase) {
        case INIT_START:
            getSettings();
            _initPhase = INIT_WAIT_SETTINGS;
            break;
            
        case INIT_WAIT_SETTINGS:
            if (_saVersion > 0) {
                if (_saVersion == 2) {
                    uint16_t getPitFreq = SA_FREQ_GETPIT;
                    uint8_t buf[7] = {
                        SA_PREAMBLE_1, SA_PREAMBLE_2,
                        (uint8_t)(SA_CMD_SET_FREQ << 1 | 1), 2,
                        (uint8_t)(getPitFreq >> 8), (uint8_t)(getPitFreq & 0xFF),
                        0
                    };
                    buf[6] = calculateCRC8(buf, 6);
                    queueCommand(buf, 7);
                    _initPhase = INIT_WAIT_PITFREQ;
                } else {
                    _initPhase = INIT_DONE;
                    _isReady = true;
                }
            }
            break;
            
        case INIT_WAIT_PITFREQ:
            if (_saPitFreq > 0) {
                _initPhase = INIT_DONE;
                _isReady = true;
            }
            break;
            
        case INIT_DONE:
            // Ready for commands
            break;
    }
    
    unsigned long now = millis();
    
    if (_outstandingCmd != SA_CMD_NONE && (now - _lastTransmission > SA_CMD_TIMEOUT)) {
        sendQueue();
    } else if (_queueHead != _queueTail) {
        sendQueue();
    } else if (_initPhase == INIT_DONE && 
               (now - _lastCommand >= SA_POLLING_INTERVAL)) {
        getSettings();
        sendQueue();
    }
}

bool SmartAudioVTX::isReady() {
    return _isReady && _saVersion > 0;
}

bool SmartAudioVTX::setFrequency(uint16_t freq) {
    uint8_t buf[7] = {
        SA_PREAMBLE_1, SA_PREAMBLE_2,
        (uint8_t)(SA_CMD_SET_FREQ << 1 | 1), 2,
        (uint8_t)(freq >> 8), (uint8_t)(freq & 0xFF),
        0
    };
    buf[6] = calculateCRC8(buf, 6);
    
    queueCommand(buf, 7);
    return true;
}

bool SmartAudioVTX::setPower(uint16_t power) {
    uint8_t buf[6] = {
        SA_PREAMBLE_1, SA_PREAMBLE_2,
        (uint8_t)(SA_CMD_SET_POWER << 1 | 1), 1,
        (uint8_t)power,
        0
    };
    buf[5] = calculateCRC8(buf, 5);
    
    queueCommand(buf, 6);
    return true;
}

bool SmartAudioVTX::setPitMode(bool enable) {
    if (_saVersion < 2) {
        return false;
    }
    
    uint8_t mode = enable ? SA_MODE_SET_IN_RANGE : SA_MODE_CLR_PITMODE;
    setMode(mode);
    return true;
}

bool SmartAudioVTX::setBandAndChannel(uint8_t band, uint8_t channel) {
    if (band < VTX_MIN_BAND || band > VTX_MAX_BAND || 
        channel < VTX_MIN_CHANNEL || channel > VTX_MAX_CHANNEL) {
        return false;
    }
    
    // Convert to device channel value (0-39)
    const uint8_t chval = (band - VTX_MIN_BAND) * VTX_MAX_CHANNEL + (channel - VTX_MIN_CHANNEL);
    
    uint8_t buf[6] = {
        SA_PREAMBLE_1, SA_PREAMBLE_2,
        (uint8_t)(SA_CMD_SET_CHAN << 1 | 1), 1,
        chval,
        0
    };
    buf[5] = calculateCRC8(buf, 5);
    
    queueCommand(buf, 6);
    return true;
}

// ===== Private Methods =====

uint8_t SmartAudioVTX::calculateCRC8(const uint8_t* data, uint8_t len) {
    
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

void SmartAudioVTX::sendFrame(uint8_t* buf, uint8_t len) {
    if (!_serial) {
        return;
    }
    
    // Send dummy byte for UART stabilization (as per esp-fc implementation)
    static const uint8_t dummyByte = 0x00;
    _serial->write(&dummyByte, 1);
    
    // Send frame
    _serial->write(buf, len);
    _serial->flush();
    
    _stats.packetsSent++;
    _lastTransmission = millis();
}

void SmartAudioVTX::queueCommand(uint8_t* buf, uint8_t len) {
    if ((_queueHead + 1) % SA_QUEUE_SIZE == _queueTail) {
        return;
    }
    
    memcpy(_cmdQueue[_queueHead].buffer, buf, len);
    _cmdQueue[_queueHead].length = len;
    _queueHead = (_queueHead + 1) % SA_QUEUE_SIZE;
}

void SmartAudioVTX::sendQueue() {
    if (_queueHead == _queueTail) {
        return;
    }
    
    Command& cmd = _cmdQueue[_queueTail];
    sendFrame(cmd.buffer, cmd.length);
    
    _outstandingCmd = cmd.buffer[2] >> 1;
    
    _queueTail = (_queueTail + 1) % SA_QUEUE_SIZE;
    _lastCommand = millis();
}

void SmartAudioVTX::getSettings() {
    uint8_t buf[5] = {
        SA_PREAMBLE_1, SA_PREAMBLE_2,
        (uint8_t)(SA_CMD_GET_SETTINGS << 1 | 1), 0,
        0
    };
    buf[4] = calculateCRC8(buf, 4);
    
    queueCommand(buf, 5);
}

void SmartAudioVTX::setMode(uint8_t mode) {
    uint8_t buf[6] = {
        SA_PREAMBLE_1, SA_PREAMBLE_2,
        (uint8_t)(SA_CMD_SET_MODE << 1 | 1), 1,
        mode,
        0
    };
    buf[5] = calculateCRC8(buf, 5);
    
    queueCommand(buf, 6);
}

void SmartAudioVTX::processResponse(uint8_t* buf, uint8_t len) {
    if (len < (SA_DATA_HEADER_SIZE + 1)) {
        return;
    }
    
    const uint8_t cmd = buf[0];
    _outstandingCmd = SA_CMD_NONE;
    
    switch (cmd) {
        case SA_CMD_GET_SETTINGS:
        case SA_CMD_GET_SETTINGS_V2:
        case SA_CMD_GET_SETTINGS_V21:
            if (len < 7) break;
            
            _saVersion = (cmd == SA_CMD_GET_SETTINGS) ? 1 :
                        (cmd == SA_CMD_GET_SETTINGS_V2) ? 2 : 3;
            _saChannel = buf[2];
            _saPower = buf[3] & SA_POWER_MASK;
            _saMode = buf[4];
            _saFreq = (buf[5] << 8) | buf[6];
            
            _stats.packetsReceived++;
            break;
            
        case SA_CMD_SET_FREQ:
            if (len < 5) break;
            {
                const uint16_t freq = (buf[2] << 8) | buf[3];
                if (freq & SA_FREQ_GETPIT) {
                    _saPitFreq = freq & 0x3FFF;
                }
            }
            break;
            
        default:
            break;
    }
}

void SmartAudioVTX::receiveChar(uint8_t c) {
    switch (_rxState) {
        case WAIT_PREAMBLE_1:
            if (c == SA_PREAMBLE_1) {
                _rxBuffer[0] = c;
                _rxPos = 1;
                _rxState = WAIT_PREAMBLE_2;
            }
            break;
            
        case WAIT_PREAMBLE_2:
            if (c == SA_PREAMBLE_2) {
                _rxBuffer[_rxPos++] = c;
                _rxState = WAIT_COMMAND;
            } else {
                _stats.badPreamble++;
                _rxState = WAIT_PREAMBLE_1;
            }
            break;
            
        case WAIT_COMMAND:
            _rxBuffer[_rxPos++] = c;
            _rxCommand = c;
            _rxState = WAIT_LENGTH;
            break;
            
        case WAIT_LENGTH:
            _rxBuffer[_rxPos++] = c;
            _rxLength = c;
            if (_rxLength == 0) {
                _rxState = WAIT_CRC;
            } else if (_rxLength > SA_MAX_PACKET_LEN - SA_DATA_HEADER_SIZE - 1) {
                _stats.badLength++;
                _rxState = WAIT_PREAMBLE_1;
            } else {
                _rxState = WAIT_DATA;
            }
            break;
            
        case WAIT_DATA:
            _rxBuffer[_rxPos++] = c;
            if (_rxPos >= SA_DATA_HEADER_SIZE + _rxLength) {
                _rxState = WAIT_CRC;
            }
            break;
            
        case WAIT_CRC:
            _rxBuffer[_rxPos++] = c;
            
            const uint8_t crc = calculateCRC8(_rxBuffer, _rxPos - 1);
            if (crc == c) {
                processResponse(_rxBuffer + 2, _rxPos - 2);
            } else {
                _stats.crcErrors++;
            }
            
            _rxState = WAIT_PREAMBLE_1;
            _rxPos = 0;
            break;
    }
}
