/*
 * animator.c
 *
 * Created: 25/10/2022 12:29:23 AM
 *  Author: LiamM
 */ 

#include <stdint.h>
#include "animator.h"
#include "pixel_colour.h"
#include "ledmatrix.h"
#include "display.h"
#include "game.h"
#include "timer0.h"

// Game over scroll text
// Each element represents column of size 8 in LED matrix display.
static uint8_t game_over_vert_text[62] = {
	0, 0, 0x40, 0x7c, 0x40, 0x4c, 0x44, 0x78, 0,	//G
	0x38, 0x44, 0x44, 0x7c, 0x44, 0x44, 0,			//A
	0x44, 0x6c, 0x54, 0x44, 0x44, 0x44, 0,			//M
	0x7c, 0x40, 0x40, 0x70, 0x40, 0x40, 0x7c, 0,	//E
	0, 0,
	0x7c, 0x44, 0x44, 0x44, 0x44, 0x7c, 0,			//O
	0x44, 0x44, 0x44, 0x44, 0x28, 0x10, 0,			//V
	0x7c, 0x40, 0x40, 0x70, 0x40, 0x40, 0x7c, 0,	//E
	0x7c, 0x44, 0x44, 0x78, 0x44, 0x44, 0			//R
};


uint8_t scroll_playing_flag = 0;
uint8_t anim_paused_flag = 0;

// Scroll animation global variables (only accessed locally)
PixelColour current_scroll_colour;
uint8_t current_scroll_head_index;
static uint8_t * current_scroll_anim;
uint8_t current_scroll_length;
uint8_t current_scroll_direction;
uint16_t current_scroll_frame_time;
uint32_t current_scroll_time;

// Move animation global variable (only accessed locally)
uint32_t current_move_time;

// Set the global variables for the current scroll animation
void set_scroll_anim(uint8_t* pixel_columns, uint8_t pixel_columns_length, PixelColour pixel_colours, uint16_t frame_time, uint8_t scroll_direction) {
	current_scroll_anim = pixel_columns;
	current_scroll_length = pixel_columns_length;
	current_scroll_colour = pixel_colours;
	current_scroll_frame_time = frame_time;
	current_scroll_time = get_current_time();
	current_scroll_direction = scroll_direction;
	current_scroll_head_index = 0;
	scroll_playing_flag = 1;
}

// Update loop for scroll animation
void scroll_anim(void) {	
	uint8_t anim_column_data = 0;
	uint8_t display_start_column = 0;
	MatrixColumn display_column_data = {0, 0, 0, 0, 0, 0, 0, 0};
	
	// Set display shift direction and starting column
	switch(current_scroll_direction) {
		case SCROLL_UP:
			display_start_column = 0;
			ledmatrix_shift_display_right();
			break;
		case SCROLL_DOWN:
			display_start_column = MATRIX_NUM_COLUMNS - 1;
			ledmatrix_shift_display_left();
			break;
	}
	
	// If the leading edge of the scroll image is less than the total columns 
	// of the animation then the entire image is not yet displayed 
	if (current_scroll_length > current_scroll_head_index) {
		// Get the current column of pixel data from the scroll animation
		anim_column_data = current_scroll_anim[current_scroll_head_index];
		
		// Iterate over the pixel array to set on/off
		for (int8_t column_index = 8; column_index > 0; column_index--) {
			// If the relevant MSB is set, we set this to a coloured pixel, else blank
			if(anim_column_data & 0x80) {
				display_column_data[column_index - 1] = current_scroll_colour;
			}
			anim_column_data <<= 1;
		}
	}
	// Set the pixels of the starting column 
	ledmatrix_update_column(display_start_column, display_column_data);
	
	// If any part of the scroll image is visible on the screen continue 
	// to iterate the leading edge index
	if (current_scroll_length + (2 * MATRIX_NUM_COLUMNS) > current_scroll_head_index) {
		current_scroll_head_index++;
	}
	else {
		current_scroll_head_index = 0;
		scroll_playing_flag = 0;
	}

}

// Initialise move animation
void set_move_anim(void) {
	current_move_time = get_current_time();
	move_anim();
}

// Continuously play animations if any are being played
void play_animations(void) {
	uint32_t current_time = get_current_time();
	
	if (!anim_paused_flag) {
		if (scroll_playing_flag) {
			if (current_time >= current_scroll_time + current_scroll_frame_time) {
				scroll_anim();
				current_scroll_time = current_time;
			}
		}
		
		if (current_time >= current_move_time + MOVE_SPEED) {
			move_anim();
			current_move_time = current_time;
		}	
	}
}

// Pause all game animations
void pause_animations(uint8_t pause_flag) {
	anim_paused_flag = pause_flag;
}

// Stop all game animations
void stop_animations(void) {
	scroll_playing_flag = 0;
}

// Play game over scroll animation
void play_game_over_anim(void) {
	set_scroll_anim(game_over_vert_text, 62, COLOUR_GREEN, 80, SCROLL_UP);
}