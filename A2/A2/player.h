/*
 * player.h
 *
 * Created: 22/10/2022 11:13:17 AM
 *  Author: LiamM
 */ 


#ifndef PLAYER_H_
#define PLAYER_H_

typedef struct{
	uint8_t x;
	uint8_t y;
	uint8_t visibility;
	uint8_t game_turns;
	uint8_t game_time;
} player;

#endif /* PLAYER_H_ */