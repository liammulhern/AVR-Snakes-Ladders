/*
 * buzzer.h
 *
 * Created: 22/10/2022 11:00:55 PM
 *  Author: LiamM
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_

#include <stdint.h>
#include "notes.h"

#define UNUSED_VAR     __attribute__ ((unused))

typedef struct {
	uint16_t frequency;
	float dutycycle;
	uint8_t	slide;
	uint32_t duration;	
} sound;

static const sound button_sound = {700, 50.0, 5, 50};
static const sound move_sound = {580, 25.0, 5, 80};
static const sound snake_sound = {780, 50.0, -5, 500};

UNUSED_VAR static sound ladder_sound[3] = {
	{380, 30.0, 5, 400},
	REST(100),
	{650, 45.0, -20, 50}
};

UNUSED_VAR static sound gameover_sound[17] = {
	  NOTE_E5(HALF), NOTE_C5(HALF), NOTE_D5(HALF), NOTE_B4(HALF), NOTE_C5(HALF), NOTE_A4(HALF),
	  NOTE_GS4(HALF), NOTE_B4(QUARTER), REST(QUARTER), NOTE_E5(HALF), NOTE_C5(HALF), NOTE_D5(HALF),
	  NOTE_B4(HALF), NOTE_C5(QUARTER), NOTE_E5(QUARTER), NOTE_A5(HALF), NOTE_GS5(HALF)
};

UNUSED_VAR static sound start_sound[38] = {
	NOTE_E5(QUARTER), NOTE_B4(EIGHTH), NOTE_C5(EIGHTH),	NOTE_D5(QUARTER), NOTE_C5(EIGHTH), NOTE_B4(EIGHTH),
	NOTE_A4(QUARTER), NOTE_A4(EIGHTH), NOTE_C5(EIGHTH),	NOTE_E5(QUARTER), NOTE_D5(EIGHTH), NOTE_C5(EIGHTH),
	NOTE_B4(DOTQUARTER), NOTE_C5(EIGHTH), NOTE_D5(QUARTER),	NOTE_E5(QUARTER), NOTE_C5(QUARTER), NOTE_A4(QUARTER),
	NOTE_A4(EIGHTH), NOTE_A4(QUARTER), NOTE_B4(EIGHTH),	NOTE_C5(EIGHTH), NOTE_D5(DOTQUARTER), NOTE_F5(EIGHTH),
	NOTE_A5(QUARTER), NOTE_G5(EIGHTH), NOTE_F5(EIGHTH),	NOTE_E5(DOTQUARTER), NOTE_C5(EIGHTH), NOTE_E5(QUARTER),
	NOTE_D5(EIGHTH), NOTE_C5(EIGHTH), NOTE_B4(QUARTER),	NOTE_B4(EIGHTH), NOTE_C5(EIGHTH), NOTE_D5(QUARTER),
	NOTE_E5(QUARTER), NOTE_C5(QUARTER)
};

void init_buzzer(void);

uint16_t freq_to_clock_period(uint16_t freq);

uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod);

void stop_timer(void);

void start_timer(void);

void play_buzzer(void);

void play_sound(sound buzzer_sound);

void play_melody(sound *buzzer_melody, uint8_t buzzer_melody_length);

void set_tone(uint16_t buzzer_frequency, float buzzer_dutycycle, int8_t buzzer_slide, uint16_t buzzer_duration);

uint8_t get_tone_playing(uint32_t current_time);

void mute_tone_toggle(void);

uint8_t get_mute_tone(void);

void set_mute_tone(uint8_t mute);

uint8_t get_game_mute_flag(void);

void set_game_mute_flag(uint8_t mute_flag);

#endif /* BUZZER_H_ */