// Helper functions
void transmit(double tx, double ty, double tz, unsigned char tc,
			 unsigned char goalLoc, unsigned char tc_g) {
		unsigned char val = 0x00;
		
		unsigned char playerLoc = (map(tx, ty, tz, tc) >> 2) & 0x0F;
		
		val = goalLoc << 4;
		val |= playerLoc;
		if (USART_IsSendReady(0)) {
			USART_Send(val, 0);
		}
}

void show () {
	unsigned char val = rand()  % (0xFF + 1 - 0x00) + 0x00; 
	if (USART_IsSendReady(0)) {
		USART_Send(val, 0);
	}
}