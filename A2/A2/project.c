/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett
 * Modified by Liam Mulhern
 */ 

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "seven_seg.h"
#include "dice.h"
#include "gameboard.h"
#include "joystick.h"
#include "buzzer.h"
#include "animator.h"

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);
void print_new_game(void);
void print_multi_player(void);
void print_start_game(void);
void print_game_over(void);
void print_difficulty(void);
void print_difficulty_time(uint16_t player_time);
void print_paused(uint8_t paused);
uint8_t handle_serial_input(char serial_input, uint8_t player_num);
uint8_t handle_button_input(uint8_t btn, uint8_t player_num);
uint8_t handle_restart_wait(char serial_input);
uint8_t handle_multi_player_input(char serial_input);
uint8_t handle_difficulty_input(char serial_input);
uint8_t handle_board_change_input(char serial_input);
uint8_t handle_audio_input(char serial_input);
uint8_t handle_pause_input(char serial_input, uint8_t btn);
uint8_t handle_joysick_input(int8_t *dx, int8_t *dy, uint8_t player_num);

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete.
	start_screen();
	
	// Loop forever and continuously play the game.
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	ledmatrix_setup();
	
	init_button_interrupts();
	init_sevenseg();
	init_dice();
	init_buzzer();
	init_joystick();
	init_timer0();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	hide_cursor();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Snakes and Ladders"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 A2 by LIAM MULHERN - 47428748"));
	
	// Output the static start screen
	start_display();
	play_melody(start_sound, 38);
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	char serial_input = get_serial();
	while(handle_restart_wait(serial_input)) {
		serial_input = get_serial();
		
		// Handle audio output change
		if (handle_audio_input(serial_input)) {
			set_game_mute_flag(get_mute_tone());
		}
	}
}

void new_game(void) {
	sevenseg_display_digit(0,0);
	
	// Clear the serial terminal
	print_new_game();
	print_multi_player();
	
	//Set game board to default level.
	init_game_board(GAMEBOARD_1);
	
	move_terminal_cursor(10,12);
	printf("Level: %d", GAMEBOARD_1);

	print_difficulty();
	
	char serial_input = get_serial();
	
	while(handle_restart_wait(serial_input)) {
		serial_input = get_serial();
		
		handle_board_change_input(serial_input);
		
		// Handle audio output change
		if (handle_audio_input(serial_input)) {
			set_game_mute_flag(get_mute_tone());
		}
		
		// Handle multiplayer select
		if (handle_multi_player_input(serial_input)) {
			print_multi_player();
		}
		
		// Handle difficulty select
		if (handle_difficulty_input(serial_input)) {
			print_difficulty();
		}
	}
	
	print_start_game();
	
	// Initialise the game and display
	init_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void) button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	// Create local game variables and timers
	uint32_t last_flash_time, last_dice_time, last_difficulty_time, pause_start_time, pause_delta_time, joystick_time, current_time;
	uint16_t joystick_delay = 0;
	uint8_t button_input;
	uint8_t dice_num = 0;
	uint8_t pause_flag = 0;
	uint8_t current_player_num = PLAYER_1;	
	int8_t current_player_dx = 0;
	int8_t current_player_dy = 0;
	char serial_input;
	
	pause_start_time = 0;
	pause_delta_time = 0;
	
	last_flash_time = get_current_time();
	last_dice_time = get_current_time();
	last_difficulty_time = get_current_time();
	joystick_time = get_current_time();
	
	// Loop game until game over is triggered
	while(!is_game_over()) {
		// Check if any button has been pushed
		button_input = button_pushed();
		// Read serial input from terminal
		serial_input = get_serial();
		// Set current time in ms
		current_time = get_current_time();
		
		// Handle game pause conditions
		if (handle_pause_input(serial_input, button_input)) {
			pause_flag = 1 - pause_flag;
			print_paused(pause_flag);
			
			if (pause_flag) {
				pause_start_time = current_time;
				set_mute_tone(1);
				pause_animations(1);
			}
			else {
				pause_delta_time = current_time - pause_start_time;
				set_mute_tone(get_game_mute_flag());
				pause_animations(0);
			}
		}
		
		if (!pause_flag) {
			sevenseg_display_digit(get_player_turns() % 10, dice_num);
			
			// Handle audio output change
			if (handle_audio_input(serial_input)) {
				set_game_mute_flag(get_mute_tone());
			}
			
			// Handle difficulty change
			if (handle_difficulty_input(serial_input)) {
				print_difficulty();
			}
			
			// Handle joystick movement
			if (current_time >= joystick_time + joystick_delay) {
				joystick_delay = 0;
				
				if (handle_joysick_input(&current_player_dx, &current_player_dy, current_player_num)) {
					set_player_visibility(1, current_player_num);
					
					if (!get_single_player()) {
						current_player_num = handle_player_num_change(current_player_num);
						joystick_delay = 800;
						set_axis_hold(0);
					}
					last_flash_time = current_time;
					last_difficulty_time = current_time;
				}
				joystick_time = current_time;
			}
		
			// Handle IO board button input
			if (handle_button_input(button_input, current_player_num)) {
				set_player_visibility(1, current_player_num);
				if (!get_single_player()) current_player_num = handle_player_num_change(current_player_num);
				last_flash_time = current_time;
				last_difficulty_time = current_time;
			}
			
			// Handle serial terminal input
			if (handle_serial_input(serial_input, current_player_num)) {
				set_player_visibility(1, current_player_num);
				last_flash_time = current_time;
				last_difficulty_time = current_time;
			}
			
			// When the dice roll finishes generate random number and print to terminal 
			if (get_dice_roll_finish()) {
				dice_num = dice_roll_rand();
			
				move_terminal_cursor(10,13);
				printf("Dice Number: %d", dice_num);
			
				move_player_n(dice_num, current_player_num);
				set_player_visibility(1, current_player_num);
			
				if (!get_single_player()) current_player_num = handle_player_num_change(current_player_num);
				last_flash_time = current_time;
				last_difficulty_time = current_time;
			}
		
			// Decrement difficulty timer every 10ms
			if (current_time >= last_difficulty_time + 10 && get_game_difficulty() != EASY)  {
				print_difficulty_time(update_player_time(current_player_num));
				last_difficulty_time = current_time;
			}
		
			// Change dice roll every 80ms
			if (current_time >= last_dice_time + 80) {
				if (get_dice_rolling()) {
					dice_num = dice_roll();
				
					move_terminal_cursor(10,13);
					printf("Dice Number: %d", dice_num);
				}
				last_dice_time = current_time;
			}
		
			// Flash player
			if (current_time >= last_flash_time + pause_delta_time + 500) {
				flash_player_cursor(current_player_num);
				pause_delta_time = 0;
				last_flash_time = current_time;
			}
		}
	}
}

// Handle game over game loop
void handle_game_over() {
	play_melody(gameover_sound, 17);
	play_game_over_anim();
	print_game_over();

	char serial_input = get_serial();
	
	while(handle_restart_wait(serial_input)) {
		serial_input = get_serial();
		
		// Handle audio output change
		if (handle_audio_input(serial_input)) {
			set_game_mute_flag(get_mute_tone());
		}
	}
	
	stop_animations();
}

// Waits for the user to press any button or s in the terminal.
uint8_t handle_restart_wait(char serial_input) {
	int8_t btn = button_pushed();
	
	// If the serial input is 's' or any button is pushed exit the loop
	if (serial_input == 's' || serial_input == 'S' || btn != NO_BUTTON_PUSHED) {
		play_sound(button_sound);
		return 0;
	}
		
	return 1;
}

// Return 1 if serial button movement is detected, else 0.
uint8_t handle_serial_input(char serial_input, uint8_t player_num) {
	uint8_t move_return = 1;
	
	// If the serial input is 'w', move player up
	if (serial_input == 'w' || serial_input == 'W') {
		move_player(0, 1, player_num, 1);
	}
	// If the serial input is 'a', move player left
	else if (serial_input == 'a' || serial_input == 'A') {
		move_player(-1, 0, player_num, 1);
	}
	// If the serial input is 's', move player down
	else if (serial_input == 's' || serial_input == 'S') {
		move_player(0, -1, player_num, 1);
	}
	// If the serial input is 'd', move player right
	else if (serial_input == 'd' || serial_input == 'D') {
		move_player(1, 0, player_num, 1);
	}
	else if (serial_input == 'r' || serial_input == 'R') {
		dice_roll_toggle();
		move_return = 0;
	}
	else move_return = 0;
	
	return move_return;
}

uint8_t handle_joysick_input(int8_t *dx, int8_t *dy, uint8_t player_num) {
	joystick_adc();

	if (handle_joystick_move(dx, dy)) {
		if (move_player(*dx, *dy, player_num, 1)) {
			return 1;
		}
	}
	return 0;
}

// Return 1 if button movement is detected, else 0.
uint8_t handle_button_input(uint8_t btn, uint8_t player_num) {
	uint8_t move_return = 1;
	
	if (btn == BUTTON0_PUSHED) {
		// If button 0 is pushed, move player 1, 1 space forward.
		move_player_n(1, player_num);
	}
	else if (btn == BUTTON1_PUSHED) {
		// If button 1 is pushed, move player 1, 2 spaces forward.
		move_player_n(2, player_num);
	}
	else if (btn == BUTTON2_PUSHED) {
		dice_roll_toggle();
		move_return = 0;
	}
	else move_return = 0;
	
	return move_return;
}

// Return 1 if difficulty is changed, else 0.
uint8_t handle_difficulty_input(char serial_input) {
	uint8_t difficulty_return = 1;
	
	if (serial_input == 'e' || serial_input == 'E') {
		set_game_difficulty(EASY);
	}
	else if (serial_input == 'm' || serial_input == 'M') {
		set_game_difficulty(MEDIUM);
	}
	else if (serial_input == 'h' || serial_input == 'H') {
		set_game_difficulty(HARD);
	}
	else difficulty_return = 0;
	
	return difficulty_return;
}

// Return 1 if game board is changed, else 0.
uint8_t handle_board_change_input(char serial_input) {
	uint8_t game_board_num;
	
	if (serial_input == 'b' || serial_input == 'B') {
		game_board_num = handle_game_board_num_change();
		
		move_terminal_cursor(10,12);
		printf("Level: %d", game_board_num);
		
		init_game_board(game_board_num);
		
		return 1;
	}
	
	return 0;
}

// Return 1 if multiplayer/singleplayer is changed, else 0.
uint8_t handle_multi_player_input(char serial_input) {
	if (serial_input == '1') {
		set_single_player(1);
		return 1;
	}
	else if (serial_input == '2') {
		set_single_player(0);
		return 1;
	}
	
	return 0;
}

// Return 1 if game mute status is changed, else 0.
uint8_t handle_audio_input(char serial_input) {
	if (serial_input == 'q' || serial_input == 'Q') {
		mute_tone_toggle();
		return 1;
	}
	return 0;
}

// Return 1 if game pause status is changed, else 0.
uint8_t handle_pause_input(char serial_input, uint8_t btn) {
	return (serial_input == 'p' || serial_input == 'P' || btn == BUTTON3_PUSHED);
}

// Print terminal UI for new game screen
void print_new_game(void) {
	clear_terminal();
	
	move_terminal_cursor(10,10);
	printf_P(PSTR("NEW GAME"));
	
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press (e)/(m)/(h) To Select Difficulty"));
	
	move_terminal_cursor(10,16);
	printf_P(PSTR("Press (b) To Select Level"));
	
	move_terminal_cursor(10,17);
	printf_P(PSTR("Press (1)/(2) To Select Multi-Player Game"));
	
	move_terminal_cursor(10,18);
	printf_P(PSTR("Press (s)/(Any Button) To Start Selected Level"));
	
	move_terminal_cursor(10,19);
	printf_P(PSTR("Press (q) To Mute Sound"));
}

// Print terminal UI for current game mode
void print_multi_player(void) {
	move_terminal_cursor(10,13);
	clear_to_end_of_line();
	if (get_single_player()) {
		printf_P(PSTR("Mode : Single-Player"));
	}
	else {
		printf_P(PSTR("Mode : Multi-Player"));
	}

}

// Print terminal UI for start game screen
void print_start_game(void) {
	clear_terminal();
		
	move_terminal_cursor(0, 0);
	printf_P(PSTR("Time Remaining: "));
	
	move_terminal_cursor(10,10);
	printf_P(PSTR("GAME START"));
	
	move_terminal_cursor(10,12);
	printf_P(PSTR("Dice: Stopped"));
		
	
	move_terminal_cursor(10,13);
	printf("Dice Number: %d", 0);
	
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press (e)/(m)/(h) To Select Difficulty"));
	
	move_terminal_cursor(10,16);
	printf_P(PSTR("Press (p)/(B3) To Pause Game"));
	
	move_terminal_cursor(10,17);
	printf_P(PSTR("Press (q) To Mute Sound"));
	
	print_difficulty();
}

// Print terminal UI for game over
void print_game_over(void) {
	clear_terminal();
	
	move_terminal_cursor(10,10);
	printf_P(PSTR("GAME OVER"));
	
	move_terminal_cursor(10,11);
	
	switch (get_game_winner()) {
		case PLAYER_1:
			printf("Player 1 Wins!");
			break;
		case PLAYER_2:
			printf("Player 2 Wins!");
			break;
	};
	
	move_terminal_cursor(10,13);
	printf_P(PSTR("Press (s)/(Any Button) To Start New Game"));
	
	move_terminal_cursor(10,14);
	printf_P(PSTR("Press (q) To Mute Sound"));
}


// Print terminal UI for current game difficulty
void print_difficulty(void) {
	uint8_t difficulty_num = get_game_difficulty();
	
	move_terminal_cursor(10,11);
	clear_to_end_of_line();
	
	switch (difficulty_num) {
		case EASY:
			printf("Difficulty: Easy");
			move_terminal_cursor(17,0);
			clear_to_end_of_line();
			break;
		case MEDIUM:
			printf("Difficulty: Medium");
			break;
		case HARD:
			printf("Difficulty: Hard");
			break;
	}
}

// Print terminal UI for current game timer 
void print_difficulty_time(uint16_t player_time) {
	uint16_t difficulty_num = get_game_difficulty();
	uint16_t time_remaining = (difficulty_num * 100) - player_time;
	
	move_terminal_cursor(17,0);
	clear_to_end_of_line();
	
	// Print game time
	if (time_remaining > 1000)	{
		printf("%d", time_remaining / 100);
	}
	else if (time_remaining >= 0 && time_remaining <= 1000)	{
		// Print game time in ms if game time is less than 10 seconds
		printf("%d:%d", time_remaining / 100, time_remaining % 100);
	}
}

// Print terminal UI for pause status
void print_paused(uint8_t paused) {
	move_terminal_cursor(10,10);
	clear_to_end_of_line();
	
	switch (paused) {
		case 0:		
			printf_P(PSTR("GAME START"));
			break;
		case 1:
			printf_P(PSTR("GAME PAUSED"));
			break;
	}
}