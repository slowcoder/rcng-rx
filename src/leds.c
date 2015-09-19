#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

#include "leds.h"

int leds_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF,ENABLE);

	// GPIO

	// PA12 = Blue
	// PF7  = Green
	// PF6  = Red
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_Init(GPIOF, &GPIO_InitStructure);


	return 0;
}

void leds_set(int bRed,int bGreen,int bBlue) {
	if( !bRed )   GPIOF->BSRR = (1<<6);
	else          GPIOF->BRR  = (1<<6); 
	if( !bGreen ) GPIOF->BSRR = (1<<7);
	else          GPIOF->BRR  = (1<<7); 
	if( !bBlue )  GPIOA->BSRR = (1<<12);
	else          GPIOA->BRR  = (1<<12); 
}