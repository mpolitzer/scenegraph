#ifndef TEXTURE_H
#define TEXTURE_H

struct texture {
	unsigned int type;
	unsigned int id;
};

struct texture *
texture_init(   struct texture *self,
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
