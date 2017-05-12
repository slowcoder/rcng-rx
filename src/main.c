#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_tim.h"

#include "servo.h"
#include "leds.h"
#include "uart.h"
#include "types.h"
#include "log.h"
#include "nrf24l01plus/nrf24.h"
#include "cli/cli.h"

osThreadId defaultTaskHandle;

void CAOS_OutputDebugString(const char *pzStr) {
        uart_puts(pzStr);
}

static void turnOff(void) {

	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD,DISABLE);
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_TS,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FLITF,DISABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,DISABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART7, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART8, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, DISABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);

}

uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

void StartDefaultTask(void const * argument) {
	uint8 bOn = 0;
	uint32 c;
	uint16 p = 0;

//	turnOff();

	leds_init();

	leds_set(0,1,0);

	uart_init();

	cli_init();

	servo_init();

	leds_set(1,0,0);

	nrf24_init();
	nrf24_config(2,5);
	nrf24_tx_address(tx_address);
	nrf24_rx_address(rx_address);

	nrf24_printDetails();

	leds_set(0,1,0);

	for(;;) {

#if 0
		leds_set(0,0,0);
#else
		if( bOn ) leds_set(0,1,1);
		else      leds_set(0,1,0);
#endif
		bOn = !bOn;

		c = TIM_GetCounter(TIM1);
		(void)c;
		//LOG("C=%lu",c);

		servo_setPos(0,p);
		p += 512;

		osDelay(500);
	}
}

int main(void) {

	SystemCoreClockUpdate();

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osKernelStart();

	// Should never reach this..
	for(;;);

	return 0;
}
