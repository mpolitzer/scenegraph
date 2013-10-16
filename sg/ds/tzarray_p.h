#ifndef TZARRAY_p_H
#define TZARRAY_p_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct tzarray_p_t {
	void * *buf;
	uint16_t n, max;
};

#endif /* TZARRAY_p_H */
