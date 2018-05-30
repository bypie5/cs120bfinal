#include "bit.h"

// xxxx   xx  xx
// column led color
unsigned char map(double x, double y, double z) {
	unsigned char val = 0x02;
	
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