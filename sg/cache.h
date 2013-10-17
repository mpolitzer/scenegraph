#ifndef CACHE_H
#define CACHE_H

#include <sg/ds/tzaa_p.h>

void cache_init(void);
void cache_put(const char *key, void *value);
void *cache_get(const char *key);

#endif /* CACHE_H */
