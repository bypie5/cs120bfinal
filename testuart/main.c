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

// Global variables
double x;
double y;
double z;
double g_x;
double g_y;
double g_z;
unsigned char g_color;

struct hc_sr04 sen_x = {0, 1};
struct hc_sr04 sen_y = {2, 3};
struct hc_sr04 sen_z = {4, 5};
	
// State machines
enum SampleSensors_SM {start_sampleSensors, poll_sampleSensor} state_sampleSensors;
enum Transmit_SM {start_transmit, send_transmit} state_transmit;

// Tick functions
void tick_sampleSensors() {
	// Transitions
	switch(state_sampleSensors) {
		case start_sampleSensors:
			state_sampleSensors = poll_sampleSensor;
		break;
		
		case poll_sampleSensor:
			state_sampleSensors = poll_sampleSensor;
		break;
		
		default:
			state_sampleSensors = start_sampleSensors;
		break;
	}
	
	// Actions
	switch(state_sampleSensors) {
		case poll_sampleSensor:
			x = readHC_SR04(sen_x.m_trig, sen_x.m_echo);
			y = readHC_SR04(sen_y.m_trig, sen_y.m_echo);
			z = readHC_SR04(sen_z.m_trig, sen_z.m_echo);
		break;
		
		default:
		break;
	}
}

void tick_transmit() {
	// Transitions
	switch(state_transmit) {
		case start_transmit:
			state_transmit = send_transmit;
		break;
		
		case send_transmit:
			state_transmit = send_transmit;
		break;
		
		default:
			state_transmit = start_transmit;
		break;
	}
	
	// Actions
	switch(state_transmit) {
		case send_transmit:
			transmit(g_x, g_y, g_z, g_color);
		break;
		
		default:
		break;
	}
}


// Helper functions
void transmit(double tx, double ty, double tz, unsigned char tc) {
	if ( USART_IsSendReady(0) ) {
		USART_Send(map(tx, ty, tz, tc), 0);
	}
}

int main(void)
{
	initUSART(0); // initializes USART0
	initUSART(1); // initializes USART1
    
	TimerSet(250);
	TimerOn();
	
	initHC_SR04();

	while (1) 
    {
		while(!TimerFlag) {}
		tick_sampleSensors();
		tick_transmit();
		
		TimerFlag = 0;

    }
}

