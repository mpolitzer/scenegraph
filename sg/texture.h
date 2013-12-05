#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>

struct texture {
	unsigned int type;
	unsigned int id;
	bool islum;
};

struct texture *
texture_init(   struct texture *self,
		unsigned int type,
		const char *texture);
struct texture *
texture_lum_init(
		struct texture *self,
		unsigned int type,
		const char *texture);
void
texture_load(
		struct texture *self);
void
texture_unload(
		struct texture *self);
int
textures_max(void);

#endif /* TEXTURE_H */
