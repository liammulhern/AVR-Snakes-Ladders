/*
 * seven_seg.h
 *
 * Created: 18/10/2022 11:01:32 PM
 * Author: LiamM
 */ 


#ifndef SEVEN_SEG_H_
#define SEVEN_SEG_H_

#include <stdint.h>

#define REFRESH_RATE 5

void init_sevenseg(void);

void sevenseg_display(void);

void sevenseg_clear(void);

void sevenseg_display_digit(uint8_t number_left, uint8_t number_right);

uint8_t find_digit(uint8_t number, uint8_t digit_pos);


#endif /* SEVEN_SEG_H_ */