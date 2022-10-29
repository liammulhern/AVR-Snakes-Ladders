/*
 * prand_number_gen.h
 *
 * Created: 18/10/2022 12:45:43 PM
 *  Author: Liam Mulhern
 */ 


#ifndef PRAND_NUMBER_GEN_H_
#define PRAND_NUMBER_GEN_H_

#include <stdint.h>

// Setup Pseudo RNG seed function.
void p_rand_seed(uint32_t num);

// Set up p_rand generator function.
uint8_t p_rand_next(uint8_t max);

#endif