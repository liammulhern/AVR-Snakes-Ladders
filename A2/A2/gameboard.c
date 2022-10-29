/*
 * gameboard.c
 *
 * Created: 20/10/2022 12:47:55 PM
 *  Author: LiamM
 */ 

#include <stdint.h>
#include "gameboard.h"
#include "display.h"
#include "game.h"

static game_board_layout game_board_1_layout[HEIGHT] = {
	{FINISH_LINE, 0, 0, 0, 0, 0, 0, 0},
	{0, SNAKE_START | 4, 0, 0, LADDER_END | 4, 0, 0, 0},
	{0, SNAKE_MIDDLE, 0, LADDER_MIDDLE, 0, 0, 0, 0},
	{0, SNAKE_MIDDLE, LADDER_START | 4, 0, 0, 0, 0, 0},
	{0, SNAKE_END | 4, 0, 0, 0, 0, SNAKE_START | 3, 0},
	{0, 0, 0, 0, LADDER_END | 3, 0, SNAKE_MIDDLE, 0},
	{SNAKE_START | 2, 0, 0, 0, LADDER_MIDDLE, 0, SNAKE_MIDDLE, 0},
	{0, SNAKE_MIDDLE, 0, 0, LADDER_START | 3, 0, SNAKE_END | 3, 0},
	{0, 0, SNAKE_END | 2, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, SNAKE_START | 1, 0, 0, 0, LADDER_END | 1},
	{0, LADDER_END | 2, 0, SNAKE_MIDDLE, 0, 0, LADDER_MIDDLE, 0},
	{0, LADDER_MIDDLE, 0, SNAKE_MIDDLE, 0, LADDER_START | 1, 0, 0},
	{0, LADDER_START | 2, 0, SNAKE_MIDDLE, 0, 0, 0, 0},
	{START_POINT, 0, 0, SNAKE_END | 1, 0, 0, 0, 0}
};

static game_board_layout game_board_2_layout[HEIGHT] = {
	{FINISH_LINE, 0, SNAKE_START | 4, 0, LADDER_END | 4, 0, 0, 0},
	{0, 0, SNAKE_MIDDLE, 0, 0, LADDER_MIDDLE, 0, 0},
	{0, 0, SNAKE_END | 4, 0, 0, 0, LADDER_START | 4, SNAKE_START | 3},
	{0, 0, 0, 0, 0, 0, SNAKE_MIDDLE, 0},
	{LADDER_END | 3, 0, 0, LADDER_END | 2, 0, SNAKE_MIDDLE, 0, 0},
	{LADDER_MIDDLE, 0, 0, 0, SNAKE_LADDER_MIDDLE, 0, 0, 0},
	{LADDER_MIDDLE, 0, 0, SNAKE_MIDDLE, 0, LADDER_MIDDLE, 0, 0},
	{LADDER_MIDDLE, 0, SNAKE_END | 3, 0, 0, 0, LADDER_MIDDLE, 0},
	{LADDER_MIDDLE, 0, 0, 0, 0, 0, SNAKE_START | 2, LADDER_START | 2},
	{LADDER_START | 3, 0, 0, 0, 0, 0, SNAKE_MIDDLE, 0},
	{0, 0, 0, 0, 0, 0, SNAKE_MIDDLE, 0},
	{0, SNAKE_START | 1, 0, LADDER_END | 1, 0, 0, SNAKE_MIDDLE, 0},
	{0, 0, SNAKE_MIDDLE, LADDER_MIDDLE, 0, 0, SNAKE_END | 2, 0},
	{0, 0, 0, SNAKE_LADDER_MIDDLE, 0, 0, 0, 0},
	{0, 0, 0, LADDER_MIDDLE, SNAKE_MIDDLE, 0, 0, 0},
	{START_POINT, 0, 0, LADDER_START | 1, 0, SNAKE_END | 1, 0, 0}
};

static game_board game_board_formatted[WIDTH];

// Select a game board layout from a board number.
game_board_layout* get_game_starting_layout(uint8_t game_board_num)  {
	switch (game_board_num) {
		case GAMEBOARD_1:
			return game_board_1_layout;
			break;
		case GAMEBOARD_2:
			return game_board_2_layout;
			break;
		default:
			return game_board_1_layout;
			break;
	}
}

// Return pointer to 2D array that represents the game board.
game_board* get_game_board(game_board_layout* starting_layout) {
	// Initialise the state of the playing_field
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			// Initialise this square based on the starting layout
			// the indices here are to ensure the starting layout
			// could be easily visualised when declared
			game_board_formatted[x][y] = starting_layout[HEIGHT - 1 - y][x];
			uint8_t board_object_type = get_object_type(starting_layout[HEIGHT - 1 - y][x]);
			update_square_colour(x, y, board_object_type);
		}
	}
	
	return game_board_formatted;
}