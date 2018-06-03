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
#include "helpers.h"
#include <stdlib.h>

// Global variables
double x;
double y;
double z;
double p_x;
double p_y;
double p_z;
unsigned char g_Loc;
unsigned char score;
unsigned char pulse;

// #define's
#define Start_Restart GetBit(~PINA, 7)
#define ThirtySeconds 120 
#define QuarterSecond 250

struct hc_sr04 sen_x = {0, 1};
struct hc_sr04 sen_y = {2, 3};
struct hc_sr04 sen_z = {4, 5};
	
// State machines
enum SampleSensors_SM {start_sampleSensors, poll_sampleSensor} state_sampleSensors;
enum Transmit_SM {start_transmit, send_transmit} state_transmit;
enum GameManager_SM {start_game, idle_game, plot_game, move_game, display_game} state_gameManager;
enum Buzzer_Sm {start_buzzer, wait_buzzer, buzz_buzzer} state_buzzer;

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
			transmit(p_x, p_y, p_z, 1, g_Loc, 2);
		break;
		
		default:
		break;
	}
}

// Local vars
unsigned char collected;
unsigned char timer;
void tick_gameManager() {
	// Transition
	switch(state_gameManager) {
		case start_game:
			state_gameManager = idle_game;
		break;
		
		case idle_game:
			LCD_DisplayString(1, "Welcome!");
			if (Start_Restart) {
				p_x = 0;
				p_y = 0;
				p_z = 0;
				g_Loc = 0;
				state_gameManager = plot_game;
			} else {
				state_gameManager = idle_game;
			}
			score = 0;
		break;
		
		case plot_game:
			if (Start_Restart) {
				state_gameManager = start_game;
			} else {
				state_gameManager = move_game;
			}
			pulse = 0x00;
		break;
		
		case move_game:
			if (collected) {
				state_gameManager = plot_game;
				pulse = 0x01;
				collected = 0;
			} else if (Start_Restart) {
				state_gameManager = start_game;
			} else if (timer >= ThirtySeconds) {
				state_gameManager = display_game;
			} else {
				state_gameManager = move_game;
			}
		break;
		
		case display_game:
			if (Start_Restart) {
				state_gameManager = idle_game;
			} else {
				state_gameManager = display_game;
			}
		break;
		
		default:
			state_gameManager = start_game;
		break;
	}
	
	// Actions
	switch(state_gameManager) {
		case idle_game:
			show();
			collected = 0;
			pulse = 0;
			score = 0;
			timer = 0;
		break;
		
		case plot_game:
			plot();
			timer++;
		break;
		
		case move_game:
			move();
			timer++;
		break;
		
		case display_game:
			display();
		break;
		
		default:
		break;
	}
}

unsigned char i;
void tick_buzzer() {
	// Transitions
	switch (state_buzzer) {
		case start_buzzer:
			state_buzzer = wait_buzzer;
			i = 0;
		break;
		
		case wait_buzzer:
			if (!pulse) {
				state_buzzer = wait_buzzer;
			} else {
				state_buzzer = buzz_buzzer;
			}
		break;
		
		case buzz_buzzer:
			if (i > 250) {
				state_buzzer = wait_buzzer;
				pulse = 0;
				i = 0;
			} else {
				state_buzzer = buzz_buzzer;
			}
		break;
		
		
		default:
		break;
	}
	
	// Actions
	switch (state_buzzer) {
		case buzz_buzzer:
			if (i%2==0)PORTA = SetBit(PORTA, 6, 1);
			else PORTA = SetBit(PORTA, 6, 0);
			i++; 
		break;
		
		default:
		break;
	}
}

// Game Manager functions
// Chose a random location for the goal
void plot() {
	int loc = rand() % (0x0F + 1 - 0x00) + 0x00; 
	g_Loc = (unsigned char) loc;
}

// Update the player position based on sensors
void move() {
	p_x = (unsigned char) x;
	p_y = (unsigned char) y;
	p_z = (unsigned char) z;
	
	unsigned char currLoc = (map(x,y,z,0) >> 2) & 0x0F;
	
	if (g_Loc == currLoc) {
		collected++;
		score++;
	}
}

// LCD stuff
void display() {
	LCD_DisplayString(1, score +'0');
}

unsigned char counter = 0x00;
int main(void)
{
	initUSART(0); // initializes USART0
	initUSART(1); // initializes USART1
    
	TimerSet(1);
	TimerOn();
	
	initHC_SR04();
	
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	LCD_init();
	
	while (1) 
    {
		while(!TimerFlag) {}
		if (counter >= QuarterSecond) {
			tick_sampleSensors();
			tick_transmit();
			tick_gameManager();
			counter = 0;
		}
		tick_buzzer();
		
		
		counter++;
		TimerFlag = 0;

    }
}

