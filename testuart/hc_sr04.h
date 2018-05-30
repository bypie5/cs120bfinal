#ifndef __HC_SR04__
#define __HC_SR04__

#include <util/delay.h>
#include "bit.h"
#include <math.h>

// Driver for HC-SR04 Distance Sensor

void initHC_SR04() {
	/*
	Trigger Pin A0 -> OUTPUT
	Echo Pin A1 -> INPUT
	0 0 0 0 0 0 E T
	x x x x x x 0 1 : DDRx
	*/
	DDRA = 0x01; PORTA = 0x02;
}


double readHC_SR04() {
	double duration = 0;
	
	PORTA = SetBit(PORTA, 0, 0);
	_delay_us(2);

	// 10 microseconds high
	PORTA = SetBit(PORTA, 0, 1);
	_delay_us(10);
	PORTA = SetBit(PORTA, 0, 0);
	
	while (!(PINA & 0x02)) {}
		
	while (PINA & 0x02) {duration++;}
	
	double distance = duration * 0.034/2.0;
	
	return duration;
}


#endif
