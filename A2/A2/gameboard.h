/*
 * gameboard.h
 *
 * Created: 20/10/2022 12:48:29 PM
 *  Author: LiamM
 */ 


#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_

#define GAMEBOARD_1 1
#define GAMEBOARD_2 2

#include <stdint.h>
#include "game.h"

typedef uint8_t game_board_layout[WIDTH];
typedef uint8_t game_board[HEIGHT];

game_board_layout* get_game_starting_layout(uint8_t game_board_num);

game_board* get_game_board(game_board_layout* starting_layout);

#endif /* GAMEBOARD_H_ */