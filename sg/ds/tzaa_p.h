#ifndef TZAA_p_H
#define TZAA_p_H
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct tzaa_p_t {
	uint16_t len, size;
	struct tzaa_p_node_t {
		struct tzaa_p_node_t *link;
		const char * key;
		void * value;
		uint16_t ref;
	} **buckets;
};

#endif /* TZAA_p_H */
