#ifndef __HC_SR04__
#define __HC_SR04__

#include <util/delay.h>
#include "bit.h"
#include <math.h>

// Driver for HC-SR04 Distance Sensor
struct hc_sr04 {
	unsigned char m_trig;
	unsigned char m_echo;
};

void initHC_SR04() {
	/*
	Trigger Pin A0 -> OUTPUT
	Echo Pin A1 -> INPUT
	0 0 0 0 0 0 E T
	x x x x x x 0 1 : DDRx
	*/
	DDRA = 0x55; PORTA = 0xAA;
}


double readHC_SR04(unsigned char m_trig, unsigned char m_echo) {
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