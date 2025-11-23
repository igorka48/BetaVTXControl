/**
 * @file BetaVTXControl.cpp
 * @brief Main class implementation (TX-only mode, no auto-detection)
 */

#include "BetaVTXControl.h"

BetaVTXControl::BetaVTXControl(VTXProtocolType protocolType) {
    _protocolType = protocolType;
    _vtx = nullptr;
}

BetaVTXControl::~BetaVTXControl() {
    if (_vtx) {
        delete _vtx;
        _vtx = nullptr;
    }
}

bool BetaVTXControl::begin(HardwareSerial* serial, uint8_t txPin, uint8_t rxPin) {
    if (!serial) {
        return false;
    }
    
    if (_protocolType == VTX_PROTOCOL_SMARTAUDIO) {
        _vtx = new SmartAudioVTX();
    } else if (_protocolType == VTX_PROTOCOL_TRAMP) {
        _vtx = new TrampVTX();
    }
    
    if (_vtx) {
        return _vtx->begin(serial, txPin, rxPin);
    }
    return false;
}

void BetaVTXControl::update() {
    if (!_vtx) {
        return;
    }
    
    _vtx->update();
}

bool BetaVTXControl::isReady() {
    return _vtx ? _vtx->isReady() : false;
}

bool BetaVTXControl::setFrequency(uint16_t freq) {
    return _vtx ? _vtx->setFrequency(freq) : false;
}

bool BetaVTXControl::setPower(uint16_t power) {
    return _vtx ? _vtx->setPower(power) : false;
}

bool BetaVTXControl::setPitMode(bool enable) {
    return _vtx ? _vtx->setPitMode(enable) : false;
}
