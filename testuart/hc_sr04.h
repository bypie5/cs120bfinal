#ifndef __HC_SR04__
#define __HC_SR04__

#include <util/delay.h>
#include "bit.h"
#include <math.h>

// Driver for HC-SR04 Distance Sensor
unsigned char m_trig = 0x00;
unsigned char m_echo = 0x00;
void initHC_SR04(unsigned char trig, unsigned char echo) {
	/*
	Trigger Pin A0 -> OUTPUT
	Echo Pin A1 -> INPUT
	0 0 0 0 0 0 E T
	x x x x x x 0 1 : DDRx
	*/
	DDRA = 0x55; PORTA = 0xAA;
	m_trig = trig;
	m_echo = echo;
}


double readHC_SR04() {
	double duration = 0;
	
	PORTA = SetBit(PORTA, m_trig, 0);
	_delay_us(2);

	// 10 microseconds high
	PORTA = SetBit(PORTA, m_trig, 1);
	_delay_us(10);
	PORTA = SetBit(PORTA, m_trig, 0);
	
	while (!GetBit(PINA, m_echo)) {}
		
	while (GetBit(PINA, m_echo)) {duration++;}
	
	double distance = duration * 0.034/2.0;
	
	return duration;
}


#endif
