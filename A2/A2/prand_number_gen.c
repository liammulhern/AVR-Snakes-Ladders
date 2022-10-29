/* We generate a pseudorandom number using the XORshift algorithm to
 * produce an 8 bit number.
 */

#include "prand_number_gen.h"
#include <stdint.h>

static uint8_t state;

// Set the p_rand seed for random number generation.
void p_rand_seed(uint32_t num) {
	// Bit mask state from 8 bits of seed (i.e clock)
	state = num & 0xFF;
}

/* Pseudo-random number generator using 8 bit XORshift.
 * max variable completes mod operation to 'limit' the max output.
 * Note: This only mimics a standard distribution for small values of max. */
uint8_t p_rand_next(uint8_t max) {
	uint8_t x = state;

	x ^= (x << 7);
	x ^= (x >> 5);
	x ^= (x << 3);
	state = x;

	return x % max;
}