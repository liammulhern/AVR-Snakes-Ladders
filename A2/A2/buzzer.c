/*
 * buzzer.c
 *
 * Created: 22/10/2022 11:00:48 PM
 *  Author: LiamM
 */ 

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "buzzer.h"
#include "timer0.h"

uint16_t clockperiod;
uint16_t pulsewidth;

float tone_dutycycle;
uint16_t frequency;
int8_t slide;

uint8_t timer_pause_flag;
uint8_t tone_mute_flag;
uint8_t game_mute_flag = 0;
uint8_t melody_playing_flag;

sound *melody_sounds;
uint8_t melody_sound_index;
uint8_t melody_length;

uint32_t tone_slide_time;
uint32_t tone_start_time;
uint16_t tone_duration;

void init_buzzer(void) {	
	// Make pin OC1B be an output
	DDRD |= (1 << DDRD4);
	
	// Timer/counter 1 Fast PWM, counting from 0 to the value in OCR1A
	// before reseting to 0. Count at 1MHz (CLK/8).
	// Output OC1B clears on compare match and set on timer/counter
	// overflow (non-inverting mode).
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);	
	
	// Initialise buzzer flags
	tone_mute_flag = 0;
	timer_pause_flag = 0;
	melody_playing_flag = 0;
}

uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
	// and ensures we do 32 bit arithmetic, not 16
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

// Stop Timer/Counter 1 Fast PWM to pause buzzer sound
void stop_timer(void) {
	TCCR1B &= ~(1 << CS10);
	timer_pause_flag = 1;
}

// Restart Timer/Counter 1 Fast PWM to continue buzzer sounds
void start_timer(void) {
	TCCR1B |= (1 << CS10);
	timer_pause_flag = 0;
}

// Update loop for buzzer sound effects
void play_buzzer(void) {
	uint32_t current_time = get_current_time();
	
	if (get_tone_playing(current_time) && !tone_mute_flag) {	
		// Slide tone effect changes frequency from initial value by (+/-) slide value
		if (current_time >= tone_slide_time + 5) {
			frequency += slide;
			
			if (frequency < 0) frequency = 0;
			
			tone_slide_time = current_time;
		}

		clockperiod = freq_to_clock_period(frequency);
		pulsewidth = duty_cycle_to_pulse_width(tone_dutycycle, clockperiod);
	}
	else if (melody_playing_flag && !tone_mute_flag) {
		// Play melody (array of sounds) if melody has not finished
		if (melody_sound_index < melody_length)	{
			melody_sound_index++;
			
			// Grab sound from melody pointer array
			sound next_sound = *(melody_sounds + melody_sound_index);
			play_sound(next_sound);
		}
		else melody_playing_flag = 0;
	}
	else if(!timer_pause_flag) {
		stop_timer();
	}
	
	// Update the PWM registers
	if(pulsewidth > 0) {
		// The compare value is one less than the number of clock cycles in the pulse width.
		OCR1B = pulsewidth - 1;
	}
	else {
		// Reset compare value
		OCR1B = 0;
	}
			
	OCR1A = clockperiod - 1;
}

// Play given sound effect 
void play_sound(sound buzzer_sound) {
	set_tone(buzzer_sound.frequency, buzzer_sound.dutycycle, buzzer_sound.slide, buzzer_sound.duration);
}

// Play given melody of sounds
void play_melody(sound *buzzer_melody, uint8_t buzzer_melody_length) {
	melody_sounds = buzzer_melody;
	melody_sound_index = 0;
	melody_length = buzzer_melody_length;
	melody_playing_flag = 1;
	
	play_sound(*(melody_sounds + melody_sound_index));
}

// Store the parts of a given sound locally and add to play queue
void set_tone(uint16_t buzzer_frequency, float buzzer_dutycycle, int8_t buzzer_slide, uint16_t buzzer_duration) {
	frequency = buzzer_frequency;
	tone_dutycycle = buzzer_dutycycle;
	slide = buzzer_slide;
	
	uint32_t current_time = get_current_time();
	
	tone_slide_time = current_time;
	tone_start_time = current_time;
	
	tone_duration = buzzer_duration;
	if (!tone_mute_flag) start_timer();
}

// Return 1 if buzzer tone is playing, otherwise 0
uint8_t get_tone_playing(uint32_t current_time) {
	return ((tone_duration + tone_start_time) > current_time);
}

// Toggle the mute status of all tones
void mute_tone_toggle(void) {
	tone_mute_flag = 1 - tone_mute_flag;
	play_sound(button_sound);
}

// Set the mute flag of the buzzer
void set_mute_tone(uint8_t mute) {
	tone_mute_flag = mute;
	play_sound(button_sound);
}

// Get the mute tone of the buzzer
uint8_t get_mute_tone(void) {
	return tone_mute_flag;
}

// Get the game muted flag (Used to return to previous status from pause menu)
uint8_t get_game_mute_flag(void) {
	return game_mute_flag;
}

// Set the game mute flag
void set_game_mute_flag(uint8_t mute_flag) {
	game_mute_flag = mute_flag;
}