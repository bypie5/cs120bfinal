#include "bit.h"

// xxxx   xx  xx
// column led color
unsigned char map(double x, double y, double z, unsigned char u_color) {
	unsigned char val = 0x00; 
	
	// Determine color
	unsigned char color = u_color;
	
	val = SetBit(val, 0, GetBit(color, 0));
	val = SetBit(val, 1, GetBit(color, 1));
	
	// Determine which column (0-3)
	unsigned char column = 0x03;
	
	// Only new two bits for now
	val = SetBit(val, 4, GetBit(column, 0));
	val = SetBit(val, 5, GetBit(column, 1));
	
	// Determine the z axis
	// 60 - 12
	unsigned char currLED = 0x00;
	if (z >= 48)
		currLED = 3;
		
	if (z < 48 && z >= 36)
		currLED = 2;
		
	if (z < 36 && z >= 24)
		currLED = 1;
		
	if (z < 24)
		currLED = 0;
		
	val = SetBit(val, 3, GetBit(currLED, 1));
	val = SetBit(val, 2, GetBit(currLED, 0));
	
	return val;
}