#include "tzarray_p.h"

static inline void tzarray_p_init(struct tzarray_p_t *a) {
	assert(a && "invalid array pointer, is NULL");
	a->buf = NULL;
	a->n = a->max = 0;
}
static inline void tzarray_p_fini(struct tzarray_p_t *a) {
	assert(a && "invalid array pointer, is NULL");
	free(a->buf); a->buf = NULL;
	a->n = a->max = 0;
}
static inline void tzarray_p_setlen(struct tzarray_p_t *a, uint16_t len) {
	assert(a && "invalid array pointer, is NULL");
	a->n = len;
}
static inline uint16_t  tzarray_p_getlen(const struct tzarray_p_t *a) {
	assert(a && "invalid array pointer, is NULL");
	return a->n;
}
static inline void tzarray_p_setp(struct tzarray_p_t *a, void * *buf) {
	assert(a && "invalid array pointer, is NULL");
	a->buf = buf;
}
static inline void *  *tzarray_p_getp(const struct tzarray_p_t *a) {
	assert(a && "invalid array pointer, is NULL");
	return a->buf;
}
static inline void *  *tzarray_p_push(struct tzarray_p_t *a) {
	assert(a && "invalid array pointer, is NULL");
	if (a->n >= a->max) { /* trigger a resize up */
		a->max = (3*a->max)+1;
		a->buf = realloc(a->buf, a->max * sizeof(void *));
	}
	return &a->buf[a->n++];
}
static inline void tzarray_p_pushp(struct tzarray_p_t *a, void * *v) {
	assert(a && "invalid array pointer, is NULL");
	if (a->n >= a->max) { /* trigger a resize up */
		a->max = (3*a->max)+1;
		a->buf = realloc(a->buf, a->max * sizeof(void *));
	}
	a->buf[a->n++] = *v;
}
static inline void tzarray_p_pushv(struct tzarray_p_t *a, void * v) {
	assert(a && "invalid array pointer, is NULL");
	if (a->n >= a->max) { /* trigger a resize up */
		a->max = (3*a->max)+1;
		a->buf = realloc(a->buf, a->max * sizeof(void *));
	}
	a->buf[a->n++] = v;
}
static inline void *   tzarray_p_pop(struct tzarray_p_t *a) {
	assert(a && "invalid array pointer, is NULL");
	void * v = a->buf[--a->n];
	if (a->max/3 && a->n < a->max/3) { /* trigger resize down */
		a->max = a->max/3;
		a->buf = realloc(a->buf, a->max * sizeof(void *));
	}
	return v;
}
static inline void tzarray_p_reserve(struct tzarray_p_t *a, uint16_t size) {
	assert(a && "invalid array pointer, is NULL");
	if (size > a->max) {
		a->max = size;
		a->buf = realloc(a->buf, a->max * sizeof(void *));
	}
}
static inline void * *tzarray_p_gettop(struct tzarray_p_t *a) {
	return &a->buf[a->n-1];
}
static inline void tzarray_p_print(struct tzarray_p_t *a) {
	printf("(%p,%d,%d)\n", a->buf, a->n, a->max);

}

#define tzarray_p_foreach_t(it_, a_, t_)\
	if ((a_)->buf) for ((it_)=(t_*)(a_)->buf;\
			(it_) < (t_*)(a_)->buf+(a_)->n; ++(it_))

#define tzarray_p_foreach(it_, a_)\
	if ((a_)->buf) for ((it_)=(a_)->buf;\
			(it_) < (a_)->buf+(a_)->n; ++(it_))
