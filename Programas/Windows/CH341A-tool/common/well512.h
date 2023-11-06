#ifndef WELL512_H
#define WELL512_H

/*
	Public domain WELL512 PRNG implementation
	http://lomont.org/papers/2008/Lomont_PRNG_2008.pdf	
*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct well512_state {
	uint32_t state[16];	///< initialize to random bits
	uint32_t index;		///< set to zero on initialization
};

/** \brief Intialize state using rand() for randomness; state may be also initialized manually */
void well512_init_with_rand(struct well512_state *s);

/** \brief Get PRNG value */
uint32_t well512(struct well512_state *s);

#ifdef __cplusplus
}
#endif

#endif
