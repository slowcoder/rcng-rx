#include <string.h>

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_misc.h"

#include "types.h"
#include "uart.h"
//#include "cli/cli.h"

#define RX_INTERRUPT

void upper_UART_ISR(int i);

#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)


void USART1_IRQHandler(void) {
	int i;

	if( USART_GetITStatus(USART1,USART_IT_RXNE) ) {
		i = USART_ReceiveData(USART1);

		USART_ClearFlag(USART1,USART_FLAG_RXNE);

		upper_UART_ISR(i);
	}

	uint32 status;
	status = USART1->ISR;
    while(status & (USART_FLAG_RXNE | USART_FLAG_ERRORS))
    {
        uint8 ch;

        ch = USART_ReceiveData(USART1);
        (void)ch;

        status = USART1->ISR;
    }

}


int uart_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// PB6 = TX
	// PB7 = RX

	// Clock the GPIO bank
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);

	// Clock the UART peripheral
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

#ifdef RX_INTERRUPT
	NVIC_InitTypeDef NVIC_InitStructure;

	memset(&NVIC_InitStructure,0,sizeof(NVIC_InitTypeDef));

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_SetPriority(USART1_IRQn,5);
#endif

	// Set up AFs
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_0);

	// Configure the GPIOs to be routed as AFs
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStruct);

	USART_ClearFlag(USART1, USART_IT_RXNE);
    USART_ClearFlag(USART1, USART_IT_TXE);
#if 0
	uint32 status;
	status = USART1->SR;
#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)
    while(status & (USART_FLAG_RXNE |    USART_FLAG_ERRORS))
    {
        uint8 ch;

        ch = USART1->DR;    // Read Data, or Clear Error(s)

        if (status & USART_FLAG_ERRORS) {
        }
        else {
//            Fifo_Insert(Uart->Rx, sizeof(ch), &ch);
        }

        status = USART1->SR;
    }
#endif

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);

#ifdef RX_INTERRUPT
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif

	return 0;
}

int uart_puts(const char *pzStr) {
	char *pS;

	pS = (char *)pzStr;
	while( *pS != 0 ) {
		USART_SendData(USART1, (uint8_t)*pS);

		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

		pS++;
	}

	return 0;
}
