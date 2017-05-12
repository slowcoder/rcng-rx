#include <string.h>

#include "stm32f0xx_pwr.h"
#include "stm32f0xx_tim.h"

#include "leds.h"
#include "types.h"

void configureTimerForRunTimeStats(void) {
    TIM_TimeBaseInitTypeDef timerInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    memset(&timerInitStructure,0,sizeof(TIM_TimeBaseInitTypeDef));

    timerInitStructure.TIM_Prescaler = 256;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 65535 * 20;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM6, &timerInitStructure);
    TIM_Cmd(TIM6, ENABLE);
}

static uint32 prev = 0,rtcnt = 0;
unsigned long getRunTimeCounterValue(void) {
	uint32 curr,delta;

	curr = TIM_GetCounter(TIM6);
	if( curr < prev ) {
		delta = 65535 - prev;
		delta += curr;
	} else {
		delta = curr - prev;
	}
	prev = curr;

	rtcnt += delta;

	return rtcnt;
}

void vApplicationIdleHook( void )
{
	//leds_set(0,0,0);
	//PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
}

void PreSleepProcessing(uint32 *ulExpectedIdleTime)
{
/* place for user code */ 
}

void PostSleepProcessing(uint32 *ulExpectedIdleTime)
{
/* place for user code */
}
