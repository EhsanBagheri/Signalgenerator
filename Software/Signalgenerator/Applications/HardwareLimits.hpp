#pragma once
#include <cstdint>

namespace HardwareLimits {

static constexpr uint32_t MinFrequency = 5000000;
static constexpr uint32_t MaxFrequency = 2100000000;
static constexpr int16_t MaxOutputLevel = 2000;
static constexpr int16_t MinOutputLevel = -6000;

static constexpr uint32_t FPGA_CLK = 200000000;
static constexpr uint32_t FIRCLKPeriods = 6;
static constexpr uint32_t MaxFIRRate = FPGA_CLK / FIRCLKPeriods;
static constexpr uint8_t FIRTaps = 71;
static constexpr uint8_t BitsModSrcPinc = 27;
static constexpr uint8_t BitsModSrcExposed = 20;
static constexpr uint8_t BitsQAMSampleratePinc = 32;
static constexpr uint8_t BitsQAMSamplerateExposed = 32;
static constexpr uint8_t BitsFMDDSPinc = 32;
static constexpr uint16_t MaxSrcValue = 4095;
static constexpr uint16_t MaxFMDeviationSetting = 65535;
static constexpr uint32_t ExtSamplerate = MaxFIRRate;
static constexpr uint32_t MaxExtModInputVoltage = 10000000;
static constexpr int32_t MaxExtModADCVoltage = 750000;
static constexpr int16_t MaxExtModADCValue = 512;

static constexpr uint32_t MaxModSrcFreq = ((uint64_t) FPGA_CLK
		* ((1ULL << BitsModSrcExposed) - 1)) / (1ULL << BitsModSrcPinc);
static constexpr uint32_t MaxFMDeviation = ((uint64_t) FPGA_CLK * MaxSrcValue
		* MaxFMDeviationSetting) / (1ULL << BitsFMDDSPinc);

}
