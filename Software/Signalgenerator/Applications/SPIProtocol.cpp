#include "SPIProtocol.hpp"
#include <cstring>
#include "common.hpp"
#include "Unit.hpp"
#include "HardwareLimits.hpp"


void Protocol::SetupModulation(FrontToRF &data, Modulation mod) {
	// clear any possible previous settings
	memset(data.modulation_registers, 0, sizeof(data.modulation_registers));
	// Setup the modulation source
	// select the source type
	data.modulation_registers[3] |= (((uint8_t) mod.source) << 8);
	switch(mod.source) {
	case SourceType::FixedValue:
		data.modulation_registers[0] |= mod.Fixed.value;
		break;
	default:
		// all other cases use a DDS to generate the source signal
		uint32_t pinc = ((uint64_t) mod.Periodic.frequency)
				* (1ULL << HardwareLimits::BitsModSrcPinc)
				/ HardwareLimits::FPGA_CLK;
		// lower 16 bits in register 0, higher 4 bits in highest nibble of register 3
		data.modulation_registers[0] |= pinc & 0xFFFF;
		data.modulation_registers[3] |= (pinc & 0xF0000) >> 4;
	}

	// Setup the modulation scheme
	// set modulation type
	uint8_t type = 0x00;
	switch(mod.type) {
	case ModulationType::AM:
		type = 0x08;
		break;
	case ModulationType::FM:
		type = 0x04;
		break;
	case ModulationType::FM_USB:
		type = 0x06;
		break;
	case ModulationType::FM_LSB:
		type = 0x05;
		break;
	case ModulationType::QAM2:
	case ModulationType::QAM4:
	case ModulationType::QAM8:
	case ModulationType::QAM16:
	case ModulationType::QAM32: {
		if(mod.QAM.differential) {
			type = 0x0D;
		} else {
			type = 0x0C;
		}
		data.modulation_registers[1] |= (uint16_t) mod.QAM.SamplesPerSymbol << 8;
		// calculate PINC
		uint64_t SamplesPerSecond = mod.QAM.SamplesPerSymbol
				* mod.QAM.SymbolsPerSecond;
		uint32_t pinc = SamplesPerSecond
				* (1ULL << HardwareLimits::BitsQAMSampleratePinc)
				/ HardwareLimits::FPGA_CLK;
		data.modulation_registers[2] |= pinc & 0xFFFF;
		data.modulation_registers[4] |= (pinc >> 16) & 0xFFFF;
	}
		break;
	case ModulationType::External:
		data.Status.ADCCouplingDC = mod.External.ACCoupled ? false : true;
		data.Status.ADCImp1M = mod.External.Impedance50R ? false : true;
		uint32_t maxADCVoltage;
		// Fullscale ADC input is 1.5V, two voltage dividers are available:
		// Range1 attenuates the signal to one fourth, Range2 to 3/50
		constexpr uint32_t ADCFullscale = 1500000;
		if (mod.External.maxVoltage > ADCFullscale * 4) {
			// needs range with maximum attenuation
			data.Status.ADCRange1 = 0;
			data.Status.ADCRange2 = 0;
			maxADCVoltage = mod.External.maxVoltage * 3 / 50;
		} else if (mod.External.maxVoltage > ADCFullscale) {
			// needs middle range
			data.Status.ADCRange1 = 1;
			data.Status.ADCRange2 = 0;
			maxADCVoltage = mod.External.maxVoltage / 4;
		} else {
			// can use no attenuation before ADC
			data.Status.ADCRange1 = 0;
			data.Status.ADCRange2 = 1;
			maxADCVoltage = mod.External.maxVoltage;
		}
		constexpr uint32_t ADCMaxValue = 511;
		data.Status.ADCMax = maxADCVoltage * ADCMaxValue / ADCFullscale;
		break;
	}
	data.modulation_registers[3] |= type;

	switch(mod.type) {
	case ModulationType::AM:
		// 0: 0% depth, 65535: 100% depth
		data.modulation_registers[1] |= common_Map(mod.AM.depth, 0, Unit::maxPercent, 0, UINT16_MAX);
		break;
	case ModulationType::FM:
	case ModulationType::FM_USB:
	case ModulationType::FM_LSB:
		// 0: 0 deviation, 65535: 6248378Hz deviation
		data.modulation_registers[1] |= common_Map(mod.FM.deviation, 0,
				HardwareLimits::MaxFMDeviation, 0,
				HardwareLimits::MaxFMDeviationSetting);
		break;
	// in QAM modulation, settings1 determines bitmask of bits per symbol
	case ModulationType::QAM2:
		data.modulation_registers[1] |= 0x0001;
		break;
	case ModulationType::QAM4:
		data.modulation_registers[1] |= 0x0003;
		break;
	case ModulationType::QAM8:
		data.modulation_registers[1] |= 0x0007;
		break;
	case ModulationType::QAM16:
		data.modulation_registers[1] |= 0x000F;
		break;
	case ModulationType::QAM32:
		data.modulation_registers[1] |= 0x001F;
		break;
	}
}
