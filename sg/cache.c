#include <string.h>
#include <sg/cache.h>
#include <sg/ds/tzaa_p.inl>

static struct tzaa_p_t _cache;

void cache_init(void) {
	tzaa_p_init(&_cache, 1);
}

void cache_put(const char *key, void *value) {
	tzaa_p_put(&_cache, key, value);
}

void *cache_get(const char *key) {
	return tzaa_p_get(&_cache, key);
}
