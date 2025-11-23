/**
 * @file VTXProtocol.cpp
 * @brief VTX protocol base class implementation with frequency tables
 * 
 * Based on Betaflight vtx_table.c
 */

#include "VTXProtocol.h"

static const uint16_t freqTableBandA[VTX_MAX_CHANNEL] = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725
};

static const uint16_t freqTableBandB[VTX_MAX_CHANNEL] = {
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866
};

static const uint16_t freqTableBandE[VTX_MAX_CHANNEL] = {
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945
};

static const uint16_t freqTableBandF[VTX_MAX_CHANNEL] = {
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880
};

static const uint16_t freqTableBandR[VTX_MAX_CHANNEL] = {
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917
};

static const uint16_t* freqTable[VTX_MAX_BAND] = {
    freqTableBandA,
    freqTableBandB,
    freqTableBandE,
    freqTableBandF,
    freqTableBandR
};

uint16_t VTXProtocol::bandChannelToFreq(uint8_t band, uint8_t channel) {
    if (band < VTX_MIN_BAND || band > VTX_MAX_BAND) {
        return 0;
    }
    if (channel < VTX_MIN_CHANNEL || channel > VTX_MAX_CHANNEL) {
        return 0;
    }
    
    const uint8_t bandIdx = band - VTX_MIN_BAND;
    const uint8_t chanIdx = channel - VTX_MIN_CHANNEL;
    
    return freqTable[bandIdx][chanIdx];
}
