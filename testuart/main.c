#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "bit.h"
#include "io.c"
#include "io.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#include "usart.h"
#include "hc_sr04.h"
#include "mapper.h"

int main(void)
{
	initUSART(0); // initializes USART0
	initUSART(1); // initializes USART1
    /* Replace with your application code */
	
	// One second period
	TimerSet(10);
	TimerOn();
	
	initHC_SR04(4,5);

	while (1) 
    {
		while(!TimerFlag) {}
		double z = readHC_SR04();
		if ( USART_IsSendReady(0) ) {
			USART_Send(map(0, 0, z, 0), 0);
		}
		TimerFlag = 0;

    }
}

