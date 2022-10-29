/*
 * game.h
 *
 * Author: Jarrod Bennett
 *
 * Function prototypes for game functions available externally. You may wish
 * to add extra function prototypes here to make other functions available to
 * other files.
 */


#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

// Game difficulties (Seconds)
// Note: 0 = no time limit
#define EASY 0
#define MEDIUM 90
#define HARD 45

// Game board dimensions
#define WIDTH  8
#define HEIGHT 16

#define MOVE_BUFFER_SIZE 24
#define MOVE_SPEED 100

// Game objects. Note upper 4 bits indicate type, lower 4 bits indicate the
// identifier number (if applicable)
#define EMPTY_SQUARE    ((uint8_t) 0x00)	// 0
#define START_POINT		((uint8_t) 0x10)	// 16
#define FINISH_LINE		((uint8_t) 0x20)	// 32
#define PLAYER_1		((uint8_t) 0x40)	// 64
#define PLAYER_2		((uint8_t) 0x50)	// 80

// Snakes and ladders are represented by a start and end, which must share a
// common identifier to generate the link. A third type is used to indicate
// the 'tunnel' that connects the start and end.
#define SNAKE_START		((uint8_t) 0x80)	// 128
#define SNAKE_END		((uint8_t) 0x90)	// 144
#define SNAKE_MIDDLE	((uint8_t) 0xA0)	// 160

#define LADDER_START	((uint8_t) 0xC0)	// 192
#define LADDER_END		((uint8_t) 0xD0)	// 208
#define LADDER_MIDDLE	((uint8_t) 0xE0)	// 224

#define SNAKE_LADDER_MIDDLE ((uint8_t) 0xF0)	// 240

typedef int8_t move_buffer[2];

// Initialise the display of the board. This creates the internal board
// and also updates the display to show the initialised board.
void init_game();

void init_player();

void init_game_board(uint8_t game_board_num);

// Return the game object at the specified position (x, y). This function does
// not consider the position of the player token since it is not stored on the
// game board.
uint8_t get_object_at(uint8_t x, uint8_t y);

// Extract the object type of a game element.
uint8_t get_object_type(uint8_t object);

// Extract the object identifier of a game element.
uint8_t get_object_identifier(uint8_t object);

// Move the player by the given number of spaces forward.
void move_player_n(uint8_t num_spaces, uint8_t player_num);

// Move the player one space in the direction (dx, dy). The player should wrap
// around the display if moved 'off' the display.
uint8_t move_player(int8_t dx, int8_t dy, uint8_t player_num, uint8_t sound_flag);

void move_player_constrain(int8_t *player_x, int8_t *player_y);

void move_buffer_reset(void);

void move_buffer_add(int8_t player_x, int8_t player_y, uint8_t player_num);

void move_buffer_remove(uint8_t player_num);

void move_anim(void);

uint8_t get_move_anim_playing(uint8_t player_num);

// Flash the player icon on and off. This should be called at a regular
// interval (see where this is called in project.c) to create a consistent
// 500 ms flash.
void flash_player_cursor(uint8_t player_num);

// Return the current number of turns.
uint8_t get_player_turns(void);

// Increment the current number of turns by num_turns.
void player_turns_inc(uint8_t num_turns);

// Sets the visibility for the player icon.
void set_player_visibility(uint8_t visible, uint8_t player_num);

// Handles object collisions when player is moved.
uint8_t handle_player_collision(int8_t player_x, int8_t player_y, uint8_t player_num);

// Returns the pointers to the coordinates (end_x, end_y) corresponding to the end of an object collision.
uint8_t collision_object_coords(uint8_t object, int8_t* end_x, int8_t* end_y);

void get_player_n_position(uint8_t player_num, int8_t* player_x, int8_t* player_y);

void set_player_n_position(uint8_t player_num, int8_t player_x, int8_t player_y);

uint8_t handle_player_num_change(uint8_t player_num);

uint8_t handle_game_board_num_change();

void set_game_difficulty(uint8_t game_difficulty_num);

uint8_t get_object_at_cursor(int8_t player_x, int8_t player_y, uint8_t player_num);

uint8_t get_game_difficulty(void);

uint16_t update_player_time(uint8_t player_num);

uint8_t get_single_player(void);

void set_single_player(uint8_t player_flag);

// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void);

uint8_t get_game_winner(void);

#endif

