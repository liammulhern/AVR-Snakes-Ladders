/*
 * dice.h
 *
 * Created: 18/10/2022 11:53:00 PM
 *  Author: LiamM
 */ 


#ifndef DICE_H_
#define DICE_H_

#include <stdint.h>

// Initialises dice hardware
void init_dice(void);

// Toggle dice roll logic on button press.
void dice_roll_toggle(void);

// Returns the incremented number between 1 and 6 for a dice roll.
uint8_t dice_roll(void);

// Returns the random number between 1 and 6 for a dice roll.
uint8_t dice_roll_rand(void);

// Returns if the dice is currently rolling.
uint8_t get_dice_rolling(void);

uint8_t get_dice_roll_finish(void);
#endif