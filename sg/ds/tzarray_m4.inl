#include "tzarray_m4.h"

static inline void tzarray_m4_init(struct tzarray_m4_t *a) {
	assert(a && "invalid array pointer, is NULL");
	a->buf = NULL;
	a->n = a->max = 0;
}
static inline void tzarray_m4_fini(struct tzarray_m4_t *a) {
	assert(a && "invalid array pointer, is NULL");
	free(a->buf); a->buf = NULL;
	a->n = a->max = 0;
}
static inline void tzarray_m4_setlen(struct tzarray_m4_t *a, uint16_t len) {
	assert(a && "invalid array pointer, is NULL");
	a->n = len;
}
static inline uint16_t  tzarray_m4_getlen(const struct tzarray_m4_t *a) {
	assert(a && "invalid array pointer, is NULL");
	return a->n;
}
static inline void tzarray_m4_setp(struct tzarray_m4_t *a, tzm4 *buf) {
	assert(a && "invalid array pointer, is NULL");
	a->buf = buf;
}
static inline tzm4  *tzarray_m4_getp(const struct tzarray_m4_t *a) {
	assert(a && "invalid array pointer, is NULL");
	return a->buf;
}
static inline tzm4  *tzarray_m4_push(struct tzarray_m4_t *a) {
	assert(a && "invalid array pointer, is NULL");
	if (a->n >= a->max) { /* trigger a resize up */
		a->max = (3*a->max)+1;
		a->buf = realloc(a->buf, a->max * sizeof(tzm4));
	}
	return &a->buf[a->n++];
}
static inline void tzarray_m4_pushp(struct tzarray_m4_t *a, tzm4 *v) {
	assert(a && "invalid array pointer, is NULL");
	if (a->n >= a->max) { /* trigger a resize up */
		a->max = (3*a->max)+1;
		a->buf = realloc(a->buf, a->max * sizeof(tzm4));
	}
	a->buf[a->n++] = *v;
}
static inline void tzarray_m4_pushv(struct tzarray_m4_t *a, tzm4 v) {
	assert(a && "invalid array pointer, is NULL");
	if (a->n >= a->max) { /* trigger a resize up */
		a->max = (3*a->max)+1;
		a->buf = realloc(a->buf, a->max * sizeof(tzm4));
	}
	a->buf[a->n++] = v;
}
static inline tzm4   tzarray_m4_pop(struct tzarray_m4_t *a) {
	assert(a && "invalid array pointer, is NULL");
	tzm4 v = a->buf[--a->n];
	if (a->max/3 && a->n < a->max/3) { /* trigger resize down */
		a->max = a->max/3;
		a->buf = realloc(a->buf, a->max * sizeof(tzm4));
	}
	return v;
}
static inline void tzarray_m4_reserve(struct tzarray_m4_t *a, uint16_t size) {
	assert(a && "invalid array pointer, is NULL");
	if (size > a->max) {
		a->max = size;
		a->buf = realloc(a->buf, a->max * sizeof(tzm4));
	}
}
static inline tzm4 *tzarray_m4_gettop(struct tzarray_m4_t *a) {
	return &a->buf[a->n-1];
}
static inline void tzarray_m4_print(struct tzarray_m4_t *a) {
	printf("(%p,%d,%d)\n", a->buf, a->n, a->max);

}

#define tzarray_m4_foreach(it_, a_)\
	for ((it_)=(a_)->buf; (size_t)(it_) < (size_t)(a_)->buf+(a_)->n; ++(it_))
