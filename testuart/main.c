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
	
	struct hc_sr04 sen_x = {0, 1};
	struct hc_sr04 sen_y = {2, 3};
	struct hc_sr04 sen_z = {4, 5};
	
	initHC_SR04();

	while (1) 
    {
		while(!TimerFlag) {}
		double x = readHC_SR04(sen_x.m_trig, sen_x.m_echo);
		double y = readHC_SR04(sen_y.m_trig, sen_y.m_echo);
		double z = readHC_SR04(sen_z.m_trig, sen_z.m_echo);
		if ( USART_IsSendReady(0) ) {
			USART_Send(map(x, y, z, 0), 0);
		}
		TimerFlag = 0;

    }
}

