#pragma once

#include "stm32f0xx_hal.h"

class MAX2871 {
public:
	constexpr MAX2871(SPI_HandleTypeDef *hspi, GPIO_TypeDef *CE, uint16_t CEpin,
			GPIO_TypeDef *LE, uint16_t LEpin, GPIO_TypeDef *MUX,
			uint16_t MUXpin, GPIO_TypeDef *RF_EN, uint16_t RF_ENpin,
			GPIO_TypeDef *LD, uint16_t LDpin) :
		regs(), f_PFD(0),
		hspi(hspi),
		CE(CE), CEpin(CEpin),
		LE(LE), LEpin(LEpin),
		MUX(MUX), MUXpin(MUXpin),
		RF_EN(RF_EN), RF_ENpin(RF_ENpin),
		LD(LD), LDpin(LDpin)
		{};

	bool Init();
	bool SetReference(uint32_t f_ref, bool doubler, uint16_t r, bool div2);
	void ChipEnable(bool on);
	void RFEnable(bool on);
	bool Locked();
	enum class Power : uint8_t {
		n4dbm = 0x00,
		n1dbm = 0x01,
		p2dbm = 0x02,
		p5dbm = 0x03,
	};
	void SetPower(Power p);
	enum class Mode : uint8_t {
		LowNoise = 0x00,
		LowSpur1 = 0x02,
		LowSpur2 = 0x03,
	};
	void SetMode(Mode m);
	enum class CPMode : uint8_t {
		Disabled = 0x00,
		CP10 = 0x01,
		CP20 = 0x02,
		CP30 = 0x03,
	};
	void SetCPMode(CPMode m);
	void SetCPCurrent(uint8_t mA);
	bool SetFrequency(uint64_t f);
	void Update();
private:
	void Write(uint8_t reg, uint32_t val);
	uint32_t regs[6];
	uint32_t f_PFD;
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *CE;
	uint16_t CEpin;
	GPIO_TypeDef *LE;
	uint16_t LEpin;
	GPIO_TypeDef *MUX;
	uint16_t MUXpin;
	GPIO_TypeDef *RF_EN;
	uint16_t RF_ENpin;
	GPIO_TypeDef *LD;
	uint16_t LDpin;
};
