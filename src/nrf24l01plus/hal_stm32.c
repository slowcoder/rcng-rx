#include "types.h"

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_spi.h"

//#include "gpio.h"
//#include "spi.h"

void nrf24_setupPins() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	// GPIO

	// PB2  = CE
	// PB10 = CSN
	// PB11 = IRQ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SPI1
#ifdef USE_HW_SPI
	SPI_InitTypeDef  SPI_InitStructure;

    // PB3 = SPI1_SCK
    // PB4 = SPI1_MISO
    // PB5 = SPI1_MOSI 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_1);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = 8;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
#else
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

#endif
}

#ifndef USE_HW_SPI
void nrf24_sck_digitalWrite(uint8_t state) {
	if( state ) {
		GPIOB->BSRR = (1<<3);
	} else {
		GPIOB->BRR = (1<<3);
	}
}
void nrf24_mosi_digitalWrite(uint8_t state) {
	if( state ) {
		GPIOB->BSRR = (1<<5);
	} else {
		GPIOB->BRR = (1<<5);
	}
}

uint8_t nrf24_miso_digitalRead(void) {
	return !!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
}
#endif

void nrf24_ce_digitalWrite(uint8 state) {
#if 1
	if( state ) {
		GPIOB->BSRR = (1<<2);
	} else {
		GPIOB->BSRR = (1<<(16+2));

	}
#else
	if( !state )
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
#endif
}

void nrf24_csn_digitalWrite(uint8 state) {
#if 1
	if( state ) {
		GPIOB->BSRR = (1<<10);
	} else {
		GPIOB->BSRR = (1<<(16+10));
		//GPIOB->BRR = (1<<6);
	}
#else
	if( !state )
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
#endif
}

#ifdef USE_HW_SPI
uint8 spi_transfer(uint8 tx) {
#if 1
	uint8_t rx;
#if 0
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_SendData8(SPI1,tx);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	rx = SPI_ReceiveData8(SPI1);
#endif
	return rx;

//	SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
#else
	uint8_t rx;

	HAL_SPI_TransmitReceive(&hspi1,&tx,&rx,1,200);

	return rx;
#endif
}
#endif