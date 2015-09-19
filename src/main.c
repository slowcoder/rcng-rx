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
#include "cli/cli.h"

osThreadId defaultTaskHandle;

void CAOS_OutputDebugString(const char *pzStr) {
        uart_puts(pzStr);
}

void StartDefaultTask(void const * argument) {
	uint8 bOn = 0;

	servo_init();
	leds_init();

	leds_set(0,1,0);

	uart_init();

	cli_init();

	for(;;) {
		//uart_puts("Tick..\r\n");

		if( bOn ) leds_set(0,1,1);
		else      leds_set(0,1,0);
		bOn = !bOn;

		osDelay(500);
	}
}

int main(void) {

	SystemCoreClockUpdate();

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 96);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osKernelStart();

	// Should never reach this..
	for(;;);

	return 0;
}
