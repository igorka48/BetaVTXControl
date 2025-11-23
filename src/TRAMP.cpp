/**
 * @file TRAMP.cpp
 * @brief TRAMP VTX protocol implementation
 * 
 * Based on Betaflight vtx_tramp.c
 * Author: jflyper (Betaflight)
 */

#include "TRAMP.h"

TrampVTX::TrampVTX() {
    memset(_txBuffer, 0, TRAMP_PACKET_SIZE);
    memset(_rxBuffer, 0, TRAMP_PACKET_SIZE);
}

TrampVTX::~TrampVTX() {
    if (_serial) {
        _serial->end();
    }
}

bool TrampVTX::begin(HardwareSerial* serial, uint8_t txPin) {
    if (!serial) {
        return false;
    }
    
    _serial = serial;
    _txPin = txPin;
    
    // TX-only mode: configure serial with TX pin only
    // Fixed baud rate 9600 as per TRAMP protocol
    if (_serial) {
        _serial->end();
    }
    _serial->setTxBufferSize(VTX_TX_BUFFER_SIZE);
    _serial->begin(TRAMP_BAUD, SERIAL_8N1, -1, txPin);  // RX=-1 (not used)
    
    _status = STATUS_OFFLINE;
    _retryCount = TRAMP_MAX_RETRIES;
    
    // In TX-only mode, we're ready immediately after begin()
    _isReady = true;
    
    return true;
}

void TrampVTX::update() {
    if (!_serial) {
        return;
    }
    
    const unsigned long now = micros();
    
    const char replyCode = receive();
    
    switch (_status) {
        case STATUS_OFFLINE:
            if (replyCode == 'r') {
                _status = STATUS_INIT;
            } else if (now - _lastRequest >= TRAMP_MIN_REQUEST_PERIOD) {
                query(TRAMP_CMD_RESET);
                _lastRequest = now;
            }
            break;
            
        case STATUS_INIT:
            if (replyCode == 'v') {
                _status = STATUS_ONLINE_MONITOR_FREQPWRPIT;
                _isReady = true;
            } else if (now - _lastRequest >= TRAMP_MIN_REQUEST_PERIOD) {
                query(TRAMP_CMD_STATUS);
                _lastRequest = now;
            }
            break;
            
        case STATUS_ONLINE_MONITOR_FREQPWRPIT:
            {
                bool configNeeded = false;
                
                if (_retryCount > 0 && now - _lastRequest >= TRAMP_MIN_REQUEST_PERIOD) {
                    if (!isRaceLocked() && _confFreq != _curFreq) {
                        sendCommand(TRAMP_CMD_SET_FREQ, _confFreq);
                        configNeeded = true;
                    } else if (!isRaceLocked() && _confPower != _curPower) {
                        sendCommand(TRAMP_CMD_SET_POWER, _confPower);
                        configNeeded = true;
                    } else if (_confPitMode != _curPitMode) {
                        sendCommand(TRAMP_CMD_SET_ACTIVE, _confPitMode ? 0 : 1);
                        configNeeded = true;
                    }
                    
                    if (configNeeded) {
                        _retryCount--;
                        _lastRequest = now;
                        _status = STATUS_ONLINE_CONFIG;
                    } else {
                        _retryCount = TRAMP_MAX_RETRIES;
                    }
                }
                
                if (!configNeeded) {
                    // Regular monitoring
                    if (now - _lastRequest >= TRAMP_STATUS_REQUEST_PERIOD) {
                        query(TRAMP_CMD_STATUS);
                        _lastRequest = now;
                    } else if (replyCode == 'v') {
                        // Got status, query temperature
                        query(TRAMP_CMD_TEMP);
                        _status = STATUS_ONLINE_MONITOR_TEMP;
                        _lastRequest = now;
                    }
                }
            }
            break;
            
        case STATUS_ONLINE_MONITOR_TEMP:
            if (replyCode == 's') {
                _status = STATUS_ONLINE_MONITOR_FREQPWRPIT;
            } else if (now - _lastRequest >= TRAMP_MIN_REQUEST_PERIOD) {
                _status = STATUS_ONLINE_MONITOR_FREQPWRPIT;
            }
            break;
            
        case STATUS_ONLINE_CONFIG:
            if (now - _lastRequest >= TRAMP_MIN_REQUEST_PERIOD) {
                query(TRAMP_CMD_STATUS);
                _status = STATUS_ONLINE_MONITOR_FREQPWRPIT;
                _lastRequest = now;
            }
            break;
    }
}

bool TrampVTX::isReady() {
    // In TX-only mode, we're ready immediately after begin()
    return _isReady;
}

bool TrampVTX::setFrequency(uint16_t freq) {
    _confFreq = freq;
    _retryCount = TRAMP_MAX_RETRIES;
    
    // In TX-only mode, send immediately
    sendCommand(TRAMP_CMD_SET_FREQ, freq);
    return true;
}

bool TrampVTX::setPower(uint16_t power) {
    _confPower = power;
    _retryCount = TRAMP_MAX_RETRIES;
    
    // In TX-only mode, send immediately
    sendCommand(TRAMP_CMD_SET_POWER, power);
    return true;
}

bool TrampVTX::setPitMode(bool enable) {
    _confPitMode = enable;
    _retryCount = TRAMP_MAX_RETRIES;
    
    // In TX-only mode, send immediately
    // TRAMP: active=1 means normal power (pit OFF), active=0 means pit mode (pit ON)
    sendCommand(TRAMP_CMD_SET_ACTIVE, enable ? 0 : 1);
    return true;
}

// ===== Private Methods =====

uint8_t TrampVTX::calculateChecksum(const uint8_t* buf) {
    uint8_t cksum = 0;
    const uint8_t checksumLength = 14;
    for (int i = 1; i < checksumLength; i++) {
        cksum += buf[i];
    }
    return cksum;
}

void TrampVTX::sendPacket(uint8_t cmd, uint16_t param) {
    if (!_serial) {
        return;
    }
    
    memset(_txBuffer, 0, TRAMP_PACKET_SIZE);
    _txBuffer[0] = TRAMP_HEADER;
    _txBuffer[1] = cmd;
    _txBuffer[2] = param & 0xFF;
    _txBuffer[3] = (param >> 8) & 0xFF;
    _txBuffer[14] = calculateChecksum(_txBuffer);
    _txBuffer[15] = 0;
    
    // Send dummy byte for UART stabilization (as per esp-fc implementation)
    static const uint8_t dummyByte = 0x00;
    _serial->write(&dummyByte, 1);
    
    // Send packet
    _serial->write(_txBuffer, TRAMP_PACKET_SIZE);
    _serial->flush();
}

void TrampVTX::sendCommand(uint8_t cmd, uint16_t param) {
    if (cmd != TRAMP_CMD_SET_ACTIVE && isRaceLocked()) {
        return;
    }
    
    sendPacket(cmd, param);
}

void TrampVTX::query(uint8_t cmd) {
    resetReceiver();
    sendPacket(cmd, 0);
}

char TrampVTX::receive() {
    if (!_serial) {
        return 0;
    }
    
    while (_serial->available()) {
        uint8_t c = _serial->read();
        _rxBuffer[_rxPos++] = c;
        
        switch (_rxState) {
            case RX_WAIT_LEN:
                if (c == 0x0F || c == 0x10) {
                    _rxState = RX_WAIT_CODE;
                } else {
                    resetReceiver();
                }
                break;
                
            case RX_WAIT_CODE:
                if (c == 'r' || c == 'v' || c == 's') {
                    _rxState = RX_DATA;
                } else {
                    resetReceiver();
                }
                break;
                
            case RX_DATA:
                if (_rxPos == TRAMP_PACKET_SIZE) {
                    const uint8_t cksum = calculateChecksum(_rxBuffer);
                    const uint8_t checksumPos = 14;
                    const uint8_t termPos = 15;
                    resetReceiver();
                    
                    if (_rxBuffer[checksumPos] == cksum && _rxBuffer[termPos] == 0) {
                        return handleResponse();
                    }
                }
                break;
        }
    }
    
    return 0;
}

char TrampVTX::handleResponse() {
    const char respCode = _rxBuffer[1];
    
    switch (respCode) {
        case 'r': {
            const uint16_t minFreq = _rxBuffer[2] | (_rxBuffer[3] << 8);
            if (minFreq != 0) {
                _minFreq = minFreq;
                _maxFreq = _rxBuffer[4] | (_rxBuffer[5] << 8);
                _maxPower = _rxBuffer[6] | (_rxBuffer[7] << 8);
                return 'r';
            }
            break;
        }
        
        case 'v': {
            const uint16_t freq = _rxBuffer[2] | (_rxBuffer[3] << 8);
            if (freq != 0) {
                _curFreq = freq;
                _curPower = _rxBuffer[4] | (_rxBuffer[5] << 8);
                _controlMode = _rxBuffer[6];
                _curPitMode = _rxBuffer[7];
                _actualPower = _rxBuffer[8] | (_rxBuffer[9] << 8);
                
                if (_confFreq == 0) {
                    _confFreq = _curFreq;
                }
                if (_confPower == 0) {
                    _confPower = _curPower;
                }
                
                return 'v';
            }
            break;
        }
        
        case 's': {
            const int16_t temp = _rxBuffer[6] | (_rxBuffer[7] << 8);
            if (temp != 0) {
                _temperature = temp;
                return 's';
            }
            break;
        }
    }
    
    return 0;
}

void TrampVTX::resetReceiver() {
    _rxState = RX_WAIT_LEN;
    _rxPos = 0;
}
