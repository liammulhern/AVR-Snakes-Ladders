/*
 * seven_seg.c
 *
 * Created: 18/10/2022 11:01:19 PM
 *  Author: LiamM
 */ 

#include <avr/io.h>
#include <stdint.h>
#include "seven_seg.h"
#include "timer0.h"

const uint8_t seven_seg[17] = {63,6,91,79,102,109,125,7,127,111,119,124,57,94,121,113,0};

uint8_t seven_seg_cc;
uint8_t seven_seg_left;
uint8_t seven_seg_right;

uint32_t last_display_time;

// Initialise hardware for seven segment display.
void init_sevenseg(void) {
	// Set port C, pins C0 : A to C7 : DP to be outputs
	DDRC = 0xFF;

	// Set port D, pin D2 to be an output for CC.
	DDRD |= (1 << DDRD2);
	
	sevenseg_display_digit(0,0);
}

// Update function for seven segment display with refresh rate
void sevenseg_display(void) {
	uint32_t current_time = get_current_time();
	
	// Update seven segment display at Refresh Rate
	if (current_time >= last_display_time + REFRESH_RATE) {
	
		if (seven_seg_cc) {
			// Write to GPIO registers to display left digit
			PORTD |= (1 << PORTD2);
			PORTC = seven_seg[seven_seg_left];
		}
		else {
			// Write to GPIO registers to display right digit
			PORTD &= ~(1 << PORTD2);
			PORTC = seven_seg[seven_seg_right];
		}
		
		// Toggle seven segment display
		seven_seg_cc = 1 - seven_seg_cc;
		
		last_display_time = current_time;
	}
}

// Clear seven segment display
void sevenseg_clear(void) {
	// digit 16 represents clear number
	sevenseg_display_digit(16, 16);
}

// Display individual digits 0 - 15 on seven segment display
void sevenseg_display_digit(uint8_t number_left, uint8_t number_right) {
	seven_seg_left = number_left;
	seven_seg_right = number_right;
}