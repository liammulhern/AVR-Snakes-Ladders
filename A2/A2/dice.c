/*
 * dice.c
 *
 * Created: 18/10/2022 11:52:52 PM
 *  Author: LiamM
 */ 

#include "dice.h"
#include "prand_number_gen.h"
#include "terminalio.h"
#include "timer0.h"
#include "buzzer.h"
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

uint8_t player_dice_roll_num;
uint8_t player_dice_roll;
uint8_t player_dice_roll_finish;

// Initialise dice hardware
void init_dice(void) {
	player_dice_roll = 0;
	// Set port D, pin D3 to be an output for dice roll status.
	DDRD |= (1 << DDRD3);
}

// Toggle dice roll logic on button press.
void dice_roll_toggle(void) {
	if (player_dice_roll) {
		// Dice is already rolling, stop dice.
		move_terminal_cursor(10,12);
		printf_P(PSTR("Dice: Stopped"));
		
		// Turn off dice indicator LED on Port D3.
		PORTD &= ~(1 << PORTD3);
		
		player_dice_roll_finish = 1;
		play_sound(button_sound);
	}
	else {
		// Dice is stopped, start rolling.
		// Seed RNG with current time.
		p_rand_seed(get_current_time());
		
		move_terminal_cursor(10,12);
		printf_P(PSTR("Dice: Rolling"));
		
		// Turn on dice indicator LED on Port D3.
		PORTD |= (1 << PORTD3);
		play_sound(button_sound);
	}
	
	player_dice_roll = 1 - player_dice_roll;
}

// Returns if the dice is currently rolling.
uint8_t get_dice_rolling(void) {
	return player_dice_roll;
}

// Returns 1 on the clock cycle when dice roll finishes
uint8_t get_dice_roll_finish(void) {
	uint8_t dice_return = player_dice_roll_finish;
	player_dice_roll_finish = 0;
	return dice_return;
}

// Returns the incremented number between 1 and 6 for a dice roll display.
uint8_t dice_roll(void) {
	player_dice_roll_num ++;
	if (player_dice_roll_num > 6) player_dice_roll_num = 1;
	return player_dice_roll_num;
}

// Returns the random number between 1 and 6 for a dice roll.
uint8_t dice_roll_rand(void) {
	return p_rand_next(6) + 1;
}
