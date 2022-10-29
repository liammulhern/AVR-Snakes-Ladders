/*
 * game.c
 *
 * Functionality related to the game state and features.
 *
 * Author: Jarrod Bennett & Liam Mulhern
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "game.h"
#include "display.h"
#include "prand_number_gen.h"
#include "gameboard.h"
#include "buzzer.h"
#include "animator.h"

static game_board* board;

// The initial game layout. Note that this is laid out in such a way that
// starting_layout[x][y] does not correspond to an (x,y) coordinate but is a
// better visual representation (but still somewhat messy).
// In our reference system, (0,0) is the bottom left, but (0,0) in this array
// is the top left.
static game_board_layout* starting_layout;

// The player is not stored in the board itself to avoid overwriting game
// elements when the player is moved.
int8_t player_1_x;
int8_t player_1_y;

// Coordinate position from player 2.
int8_t player_2_x;
int8_t player_2_y;

move_buffer player_1_move_buffer[MOVE_BUFFER_SIZE];
move_buffer player_2_move_buffer[MOVE_BUFFER_SIZE];

int8_t player_1_move_buffer_index;
int8_t player_2_move_buffer_index;

uint16_t player_1_time;
uint16_t player_2_time;

// For flashing the player icon
uint8_t player_visible;

// Total turns made by players.
uint8_t player_turns;

uint8_t game_board_number = GAMEBOARD_1;
uint8_t game_time_limit = EASY;

uint8_t game_winner;

uint8_t single_player_flag = 1;

void init_game() {		
	// start the player icon at the bottom left of the display
	// NOTE: (for INternal students) the LED matrix uses a different coordinate
	// system

	init_player();
	
	update_square_colour(player_1_x, player_1_y, PLAYER_1);
		
	if (!single_player_flag) update_square_colour(player_2_x, player_2_y, PLAYER_2);
}

void init_player() {
	player_1_x = 0;
	player_1_y = 0;
	
	player_2_x = 0;
	player_2_y = 0;
	
	player_1_time = 0;
	player_2_time = 0;
	
	player_visible = 0;
	player_turns = 0;
	
	player_1_move_buffer_index = 0;
	player_2_move_buffer_index = 0;
}

void init_game_board(uint8_t game_board_num) {
	// initialise the display we are using.
	initialise_display();
	
	starting_layout = get_game_starting_layout(game_board_num);
	board = get_game_board(starting_layout);
}

// Return the game object at the specified position (x, y). This function does
// not consider the position of the player token since it is not stored on the
// game board.
uint8_t get_object_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered empty
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
		return EMPTY_SQUARE;
	} else {
		//if in the bounds, just index into the array
		return *(*(board + x) + y);
	}
}

// Extract the object type of a game element (the upper 4 bits).
uint8_t get_object_type(uint8_t object) {
	return object & 0xF0;
}

// Get the identifier of a game object (the lower 4 bits). Not all objects
// have an identifier, in which case 0 will be returned.
uint8_t get_object_identifier(uint8_t object) {
	return object & 0x0F;
}

// Move the player by the given number of spaces forward.
void move_player_n(uint8_t num_spaces, uint8_t player_num) {
	// Create temporary x, y coord that is set depending on player number.
	int8_t player_x, player_y;
	get_player_n_position(player_num, &player_x, &player_y);
	
	move_buffer_add(player_x, player_y, player_num);
	
	//uint8_t object_at_cursor = get_object_at_cursor(player_x, player_y, player_num);
	//update_square_colour(player_x, player_y, object_at_cursor);
	
	for(int moves = 0; moves < num_spaces; moves++) {
		// Player is on odd row move left.
		if (player_y % 2) {
			if (player_x - 1 >= 0) player_x--;
			else player_y++;
		}
		// Player is on even row move right.
		else {
			if (player_x + 1 < WIDTH) player_x++;
			else player_y++;
		}
		
		if (player_y >= HEIGHT) {
			player_y = HEIGHT - 1; //Move player to top row
			player_x = (WIDTH - 1) * !((HEIGHT - 1) % 2); //Move player to top corner depending on odd/even row.
			break;
		}
		
		move_buffer_add(player_x, player_y, player_num);
	}
	
	// Player is outside bounds, move to correct top corner.
	if (player_y < HEIGHT) player_turns_inc(1);
	
	play_sound(move_sound);
	set_player_n_position(player_num, player_x, player_y);
	
	set_move_anim();
	
	handle_player_collision(player_x, player_y, player_num);
}

// Move the player one space in the direction (dx, dy). The player should wrap
// around the display if moved 'off' the display. Returns 1 if player collided with snake or ladder
uint8_t move_player(int8_t dx, int8_t dy, uint8_t player_num, uint8_t sound_flag) {
	int8_t player_x, player_y;
	int8_t temp_dx = 0;
	int8_t temp_dy = 0;
	
	get_player_n_position(player_num, &player_x, &player_y);
	
	uint8_t object_at_cursor = get_object_at_cursor(player_x, player_y, player_num);
	update_square_colour(player_x, player_y, object_at_cursor);
	
	// Player has move delta more than 1 position
	if (abs(dx) > 1 || abs(dy) > 1) {
		move_buffer_add(player_x, player_y, player_num);
		while(1) {
			if(abs(temp_dx) >= abs(dx) && abs(temp_dy) >= abs(dy)) break;
			
			if(dx > temp_dx) temp_dx++;
			else if (dx < temp_dx) temp_dx--;
			
			if(dy > temp_dy) temp_dy++;
			else if (dy < temp_dy) temp_dy--;
			
			int8_t temp_x, temp_y;
			temp_x = player_x + temp_dx;
			temp_y = player_y + temp_dy;
			
			move_player_constrain(&temp_x, &temp_y);
			
			move_buffer_add(temp_x, temp_y, player_num);
		}
	}

	player_x += dx;
	player_y += dy;
	
	move_player_constrain(&player_x, &player_y);
	
	if(sound_flag) play_sound(move_sound);
	
	set_player_n_position(player_num, player_x, player_y);
	
	// Instantly move player if move delta is 1
	if (abs(dx) == 1 || abs(dy) == 1) {
		update_square_colour(player_x, player_y, player_num);
	}

	set_move_anim();
	
	return handle_player_collision(player_x, player_y, player_num);
}

// Stop player exceeding screen bounds
void move_player_constrain(int8_t *player_x, int8_t *player_y) {
	int8_t x_pos, y_pos;
	x_pos = *player_x;
	y_pos = *player_y;
	
	if (y_pos >= HEIGHT) {
		*player_y = HEIGHT - y_pos; //Move player to bottom row.
	}
	else if (y_pos < 0) {
		*player_y = HEIGHT + y_pos; //Move player to bottom row.
	}
		
	if (x_pos >= WIDTH) {
		*player_x = WIDTH - x_pos; //Move player to left column.
	}
	else if (x_pos < 0) {
		*player_x = WIDTH + x_pos; //Move player to right column.
	}
}

// Reset move animation buffer
void move_buffer_reset(void) {
	for (uint8_t i = 0; i < MOVE_BUFFER_SIZE; i++) {
		player_1_move_buffer[i][0] = 0;
		player_1_move_buffer[i][1] = 0;
	}
}

// Return (x,y) coordinate from topmost element of move buffer
void get_move_buffer(int8_t *player_x, int8_t *player_y, uint8_t player_num) {
	switch (player_num) {
		case PLAYER_1:
		// Add player 1 position to bottom of buffer
		*player_x = player_1_move_buffer[0][0];
		*player_y = player_1_move_buffer[0][1];
		break;
		case PLAYER_2:
		// Add player 2 position to bottom of buffer
		*player_x = player_2_move_buffer[0][0];
		*player_y = player_2_move_buffer[0][1];
		break;
	}
}

// Add new (x,y) coordinate to bottom of move buffer
void move_buffer_add(int8_t player_x, int8_t player_y, uint8_t player_num) {
	int8_t temp_x, temp_y;
	
	if (player_2_move_buffer_index >= MOVE_BUFFER_SIZE - 1 ||
		player_1_move_buffer_index >= MOVE_BUFFER_SIZE - 1) {
		get_move_buffer(&temp_x, &temp_y, player_num);
		uint8_t object_at_cursor = get_object_at_cursor(temp_x, temp_y, player_num);
		update_square_colour(temp_x, temp_y, object_at_cursor);
		move_buffer_remove(player_num);
	}
	
	switch (player_num) {
		case PLAYER_1:
			// Add player 1 position to bottom of buffer
			player_1_move_buffer[player_1_move_buffer_index][0] = player_x;
			player_1_move_buffer[player_1_move_buffer_index][1] = player_y;
			player_1_move_buffer_index++;
			break;
		case PLAYER_2:
			// Add player 2 position to bottom of buffer
			player_2_move_buffer[player_2_move_buffer_index][0] = player_x;
			player_2_move_buffer[player_2_move_buffer_index][1] = player_y;
			player_2_move_buffer_index++;
			break;
	}
}


// Remove topmost element of move buffer
void move_buffer_remove(uint8_t player_num) {
	switch (player_num) {
		case PLAYER_1:
		// Remove player 1 position from top of buffer
		for (uint8_t i = 0; i < player_1_move_buffer_index; i++) {
				player_1_move_buffer[i][0] = player_1_move_buffer[i+1][0];
				player_1_move_buffer[i][1] = player_1_move_buffer[i+1][1];
		}
		player_1_move_buffer_index --;
		break;
		case PLAYER_2:
		// Remove player 2 position from top of buffer
		for (uint8_t i = 0; i < player_2_move_buffer_index; i++) {
			player_2_move_buffer[i][0] = player_2_move_buffer[i+1][0];
			player_2_move_buffer[i][1] = player_2_move_buffer[i+1][1];
		}
		player_2_move_buffer_index --;
		break;
	}
}

// Animate the movement of players
void move_anim(void) {
	int8_t pixel_x, pixel_y;
	
	if(player_1_move_buffer_index > 0) {
		get_move_buffer(&pixel_x, &pixel_y, PLAYER_1);

		// Set previous position to default board object
		uint8_t object_at_cursor = get_object_at_cursor(pixel_x, pixel_y, PLAYER_1);
		update_square_colour(pixel_x, pixel_y, object_at_cursor);
			
		move_buffer_remove(PLAYER_1);
		set_player_visibility(1, PLAYER_1);
	}
	
	if(player_2_move_buffer_index > 0) {
		get_move_buffer(&pixel_x, &pixel_y, PLAYER_2);
		
		// Set previous position to default board object
		uint8_t object_at_cursor = get_object_at_cursor(pixel_x, pixel_y, PLAYER_2);
		update_square_colour(pixel_x, pixel_y, object_at_cursor);
		
		move_buffer_remove(PLAYER_2);
		set_player_visibility(1, PLAYER_2);	
	}
	
	if(player_1_move_buffer_index > 1) {
		// Set current position to player 
		get_move_buffer(&pixel_x, &pixel_y, PLAYER_1);
		update_square_colour(pixel_x, pixel_y, PLAYER_1);
	} 
	
	if(player_2_move_buffer_index > 1) {	
		// Set current position to player
		get_move_buffer(&pixel_x, &pixel_y, PLAYER_2);	
		update_square_colour(pixel_x, pixel_y, PLAYER_2);
	}
}

// Return 1 if move anim is playing
uint8_t get_move_anim_playing(uint8_t player_num) {
	switch (player_num) {
		case PLAYER_1:
			return (player_1_move_buffer_index > 1);
		case PLAYER_2:
			return (player_2_move_buffer_index > 1);
		default:
			return 0;
	}
}

// Flash the selected player icon on and off. This should be called at a regular
// interval (see where this is called in project.c) to create a consistent
// 500 ms flash.
void flash_player_cursor(uint8_t player_num) {
	if (get_move_anim_playing(player_num)) return;
		
	int8_t player_x, player_y;

	get_player_n_position(player_num, &player_x, &player_y);
	
	if (player_visible) {
		// we need to flash the player off, it should be replaced by
		// the colour of the object which is at that location
		uint8_t object_at_cursor = get_object_at_cursor(player_x, player_y, player_num);
		
		update_square_colour(player_x, player_y, object_at_cursor);
		
	} else {
		// we need to flash the player on
		update_square_colour(player_x, player_y, player_num);
	}
	player_visible = 1 - player_visible; //alternate between 0 and 1
}

// Set the visibility of the player icon.
void set_player_visibility(uint8_t visible, uint8_t player_num) {
	if (get_move_anim_playing(player_num)) return;
	
	int8_t player_x, player_y;
	
	get_player_n_position(player_num, &player_x, &player_y);
	
	player_visible = visible;
	
	if (player_visible) {
		update_square_colour(player_x, player_y, player_num);
	} 
	else {
		uint8_t object_at_cursor = get_object_at(player_x, player_y);
		update_square_colour(player_x, player_y, object_at_cursor);
	}
}

// Return the current number of turns.
uint8_t get_player_turns(void) {
	return player_turns;
}

// Increment the current number of turns by num_turns.
void player_turns_inc(uint8_t num_turns) {
	player_turns += num_turns;
}

// Handles object collisions when player is moved. Returns 1 is object collides with snake or ladder
uint8_t handle_player_collision(int8_t player_x, int8_t player_y, uint8_t player_num) {
	uint8_t object_at_cursor =	get_object_at(player_x, player_y);
	int8_t x, y, dx, dy;
	
	x = player_x;
	y = player_y;
	
	// Pass pointer to x and y to function so they can be set externally
	uint8_t object_type = collision_object_coords(object_at_cursor, &x, &y);
	
	// Play respective sounds if collision is with snake or ladder start
	switch (object_type) {
		case SNAKE_START:
			play_sound(snake_sound);
			break;
		case LADDER_START:
			play_melody(ladder_sound, 3);
			break;
	}
	
	dx = x - player_x;
	dy = y - player_y;
	
	if (dx != 0 || dy != 0) {
		move_player(dx, dy, player_num, 0);
		return 1;
	}
	return 0;
}

// Returns the object type hit and pointers to the coordinates (end_x, end_y) corresponding to the end of an object collision.
uint8_t collision_object_coords(uint8_t object, int8_t* end_x, int8_t* end_y) {
	uint8_t object_identifier = get_object_identifier(object);
	uint8_t object_type = get_object_type(object);
	
	if (object_type == SNAKE_START || object_type == LADDER_START) {

		uint8_t loop_break_flag = 0;
		
		// Iterate over every board object and determine if it is the end of a given object.
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				uint8_t board_object = get_object_at(x, y);
				uint8_t board_object_identifier = get_object_identifier(board_object);
				uint8_t board_object_type = get_object_type(board_object);
				
				// Determine if object at (x,y) is the corresponding end element (i.e start_object_type + 0x10) for a given identifier.
				if((object_type + 0x10) == board_object_type && object_identifier == board_object_identifier) {
					*end_x = x;
					*end_y = y;
					loop_break_flag = 1;
					break;
				}
			}
			if (loop_break_flag) break;
		}
	}
	
	return object_type;
}

// Get the coordinates for a given player
void get_player_n_position(uint8_t player_num, int8_t* player_x, int8_t* player_y) {
	switch (player_num) {
		case PLAYER_1:
			*player_x = player_1_x;
			*player_y = player_1_y;
			break;
		case PLAYER_2:
			*player_x = player_2_x;
			*player_y = player_2_y;
			break;
	}
}

// Set the coordinates for a given player
void set_player_n_position(uint8_t player_num, int8_t player_x, int8_t player_y) {
	switch (player_num) {
		case PLAYER_1:
			player_1_x = player_x;
			player_1_y = player_y;
			break;
		case PLAYER_2:
			player_2_x = player_x;
			player_2_y = player_y;
			break;
	}
}

// Get object or player at (x,y) coordinate
uint8_t get_object_at_cursor(int8_t player_x, int8_t player_y, uint8_t player_num) {
	if (((player_x == player_2_x && player_y == player_2_y && player_num != PLAYER_2) ||
		(player_x == player_1_x && player_y == player_1_y && player_num != PLAYER_1)) &&
		!single_player_flag) {
		return handle_player_num_change(player_num);
	}
	else {
		return get_object_at(player_x, player_y);
	}
}

// Return the next player number
uint8_t handle_player_num_change(uint8_t player_num) {
	switch (player_num) {
		case PLAYER_1:
			return PLAYER_2;
		case PLAYER_2:
			return PLAYER_1;
		default:
			return player_num;	
	}
}

// Change game boards, return selected board number
uint8_t handle_game_board_num_change() {
	switch (game_board_number) {
		case  GAMEBOARD_1:
			game_board_number = GAMEBOARD_2;
			break;
		case GAMEBOARD_2:
			game_board_number = GAMEBOARD_1;
			break;
	}
	
	play_sound(button_sound);
	return game_board_number;
}

// Set the game difficulty (easy/medium/hard)
void set_game_difficulty(uint8_t game_difficulty_num) {
	player_1_time = 0;
	player_2_time = 0;
	game_time_limit = game_difficulty_num;
	play_sound(button_sound);
}

// Get the current game difficulty (easy/medium/hard)
uint8_t get_game_difficulty(void) {
	return game_time_limit;
}

// Set the current game mode (multi/single player)
void set_single_player(uint8_t player_flag) {
	single_player_flag = player_flag;
	play_sound(button_sound);
}

// Get the current game mode (multi/single player)
uint8_t get_single_player(void) {
	return single_player_flag;
}

//Update player timer
uint16_t update_player_time(uint8_t player_num) {
	switch (player_num) {
		case PLAYER_1:
			player_1_time ++;
			return player_1_time;
		case PLAYER_2:
			player_2_time ++;
			return player_2_time;
		default:
			return 0;
	}
}

// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void) {
	// Detect if the game is over i.e. if a player has won.
	uint8_t object_at_cursor_p1 = get_object_type(get_object_at(player_1_x, player_1_y));
	uint8_t object_at_cursor_p2 = get_object_type(get_object_at(player_2_x, player_2_y));
	
	if (object_at_cursor_p1 == FINISH_LINE || (player_2_time >= game_time_limit * 100 && game_time_limit != EASY)) {
		game_winner = PLAYER_1;
		return 1;
	}
	else if (object_at_cursor_p2 == FINISH_LINE || (player_1_time >= game_time_limit * 100 && game_time_limit != EASY)) {
		game_winner = PLAYER_2;
		return 1;
	}
	
	return 0;
}

// Returns the game winner's player number
uint8_t get_game_winner(void) {
	return game_winner;
}