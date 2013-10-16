#ifndef TZARRAY_m4_H
#define TZARRAY_m4_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct tzarray_m4_t {
	tzm4 *buf;
	uint16_t n, max;
};

#endif /* TZARRAY_m4_H */
