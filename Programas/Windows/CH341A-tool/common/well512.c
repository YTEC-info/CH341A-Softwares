#include "well512.h"
#include <stdlib.h>

void well512_init_with_rand(struct well512_state *s)
{
	unsigned int i;
	for (i=0; i<sizeof(s->state)/sizeof(s->state[0]); i++)
	{
    	s->state[i] = (rand() << 16) | rand(); 
	}
	s->index = 0;
}

uint32_t well512(struct well512_state *s)
{
	uint32_t a, b, c, d;
	uint32_t *state = s->state;

	a = state[s->index];
	c = state[(s->index+13)&15];
	b = a^c^(a<<16)^(c<<15);
	c = state[(s->index+9)&15];
	c ^= (c>>11);
	a = state[s->index] = b^c;
	d = a^((a<<5)&0xDA442D24UL);
	s->index = (s->index + 15)&15;
	a = state[s->index];
	state[s->index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
	return state[s->index];
}
