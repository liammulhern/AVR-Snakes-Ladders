/*
 * animators.h
 *
 * Created: 25/10/2022 12:29:35 AM
 *  Author: LiamM
 */ 


#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#define SCROLL_UP 0
#define SCROLL_DOWN 1
#define SCROLL_LEFT 2
#define SCROLL_RIGHT 3

#include <stdint.h>
#include "game.h"
#include "ledmatrix.h"
#include "pixel_colour.h"

void set_scroll_anim(uint8_t* pixel_columns, uint8_t pixel_columns_length,  PixelColour pixel_colours, uint16_t frame_time, uint8_t scroll_direction);

void scroll_anim(void);

void set_move_anim(void);

void play_animations(void);

void stop_animations(void);

void pause_animations(uint8_t pause_flag);

void play_game_over_anim(void);

void play_stripe_anim(void);

#endif /* ANIMATORS_H_ */