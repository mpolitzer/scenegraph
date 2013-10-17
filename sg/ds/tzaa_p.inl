#include "tzaa_p.h"

static uint16_t tzaa_p_hash(const char *key) {
	uint16_t hash=0;
	while (*key) {
		hash ^= *key + hash;
		key++;
	}
	return hash;
}
static inline void tzaa_p_init(struct tzaa_p_t *aa, uint16_t hint) {
	uint16_t i;
	static int primes[] = {
		509, 509, 1021, 2053,
		4093, 8191, 16381,
		32771, 65521, INT_MAX };
	assert(aa && "invalid associative array pointer, it NULL");
	assert(hint >= 0);
	for (i=1; primes[i] < hint; i++) {}
	aa->buckets = malloc(primes[i-1]*sizeof(aa->buckets[0]));
	aa->size = primes[i-1];
	for (i=0; i<aa->size; i++)
		aa->buckets[i] = NULL;
	aa->len = 0;
}
static inline void tzaa_p_fini(struct tzaa_p_t *aa) {
	assert(aa && "invalid associative array pointer, it NULL");
	free(aa->buckets);
}
static inline void tzaa_p_put(struct tzaa_p_t *aa, const char * key, void * value) {
	assert(aa && "invalid associative array pointer, it NULL");
	uint16_t i;
	struct tzaa_p_node_t *p;
	i = tzaa_p_hash(key)%aa->size;
	for (p = aa->buckets[i]; p; p = p->link) {
		if (strcmp(key, p->key) == 0)
			break;
	}
	if (p == NULL) {
		p = malloc(sizeof(*p));
		p->key = key;
		p->link = aa->buckets[i];
		aa->buckets[i] = p;
		aa->len++;
	}
	p->value = value;
	p->ref++;
}
static inline void *tzaa_p_get(struct tzaa_p_t *aa, const char *key) {
	uint16_t i;
	struct tzaa_p_node_t *p;
	assert(aa && "invalid associative array pointer, it NULL");
	i = tzaa_p_hash(key)%aa->size;
	for (p = aa->buckets[i]; p; p = p->link) {
		if (strcmp(key, p->key) == 0)
			break;
	}
	return p ? p->value : NULL;
}
static inline void *tzaa_p_rm(struct tzaa_p_t *aa, const char *key) {
	uint16_t i;
	struct tzaa_p_node_t **pp;
	void *value;
	assert(aa && "invalid associative array pointer, it NULL");
	i = tzaa_p_hash(key)%aa->size;
	for (pp = &aa->buckets[i]; *pp; pp = &(*pp)->link) {
		if (strcmp(key, (*pp)->key) == 0 && (--(*pp)->ref == 0)) {
			struct tzaa_p_node_t *p = *pp;
			value = p->value;
			*pp = p->link;
			free(p);
			aa->len--;
		}
	}
	return value;
}
static inline uint16_t tzaa_p_len(struct tzaa_p_t *aa) {
	assert(aa && "invalid associative array pointer, it NULL");
	return aa->len;
}
