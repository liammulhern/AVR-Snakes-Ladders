/*
 * joystick.c
 *
 * Created: 23/10/2022 10:23:45 PM
 *  Author: LiamM
 */ 

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "joystick.h"
#include "timer0.h"

uint8_t axis_toggle;
uint16_t adc_read;

uint16_t x_joy;
uint16_t y_joy;

int8_t dx_joy;
int8_t dy_joy;

uint8_t axis_hold_flag;
uint32_t axis_hold_time;
uint32_t axis_debounce_time;
uint8_t axis_hold_sample;
uint16_t axis_hold_wait;

void init_joystick(void) {
	// Set up ADC - AVCC reference, right adjust
	ADMUX = (1<<REFS0);
	
	// Start ADC, select clock div of 64 (i.e 125kHz)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
	
	axis_toggle = 0;
	axis_hold_flag = 0;
}

// Read joystick inputs from ADC
void joystick_adc() {
	// Read joystick axises
	if(axis_toggle) {
		ADMUX &= ~1;
	} 
	else {
		ADMUX |= 1;
	}
	
	// Start the ADC conversion
	ADCSRA |= (1<<ADSC);
	
	while(ADCSRA & (1<<ADSC)) {
		; /* Wait until conversion finished */
	}
	
	adc_read = ADC; // read the value

	// Set the read value to the corresponding asix value
	if(axis_toggle) {
		x_joy = adc_read;
	} 
	else {
		y_joy = adc_read;
	}

	axis_toggle = 1 - axis_toggle;
}

// Return joystick axises
void get_joystick_axis(uint16_t *x, uint16_t *y) {
	*x = x_joy;
	*y = y_joy;
}

// Set the joystick hold flag to reset hold time
void set_axis_hold(uint8_t hold_flag) {
	axis_hold_flag = hold_flag;
	axis_hold_sample = 0;
	axis_hold_wait = SENSITIVITY_INIT;
}

// Return dy, dx if joystick position has changed
uint8_t handle_joystick_move(int8_t *dx, int8_t *dy) {
	uint8_t joystick_return = 0;
	
	// Detect if joystick value is outside of dead zone.
	if (x_joy > CENTRE_X + DEADZONE_X) {
		dx_joy = -1;
	}
	else if (x_joy < CENTRE_X - DEADZONE_X) {
		dx_joy = 1;
	}
	else dx_joy = 0;
	
	if (y_joy > CENTRE_Y + DEADZONE_Y) {
		dy_joy = 1;
	}
	else if (y_joy < CENTRE_Y - DEADZONE_Y) {
		dy_joy = -1;
	}
	else dy_joy = 0;
	
	uint32_t current_time = get_current_time();
	
	if (dx_joy == *dx && dy_joy == *dy && (dx_joy != 0 || dy_joy != 0)) {
		// Sample joystick and increment when previous and current position is the same
		// This debounces the input when difference changes quickly
		if (current_time >= axis_debounce_time + 2) {
			if (!axis_hold_flag && axis_hold_sample < 255) {
				axis_hold_sample ++;
			}
			axis_debounce_time = current_time;
		}
		
		// If the sample has been the same for the past 40 sample then move in direction
		if (!axis_hold_flag && axis_hold_sample > 40) {
			axis_hold_flag = 1;
			joystick_return = 1;
			axis_hold_time = current_time;
		}
		
		// Continue to move quickly after waiting for hold
		if (axis_hold_flag && current_time >= axis_hold_time + axis_hold_wait) {
			joystick_return = 1;
			axis_hold_time = current_time;
			axis_hold_wait = SENSITIVITY_MOVE;
		}
	}
	else {
		axis_hold_flag = 0;
		axis_hold_sample = 0;
		axis_hold_wait = SENSITIVITY_INIT;
	}	

	*dx = dx_joy;
	*dy = dy_joy;
	
	return joystick_return;
}