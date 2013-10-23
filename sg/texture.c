#include <sg/texture.h>
#include <sg/cache.h>
#include <sg/gl/backend.h>
#include <SOIL.h>

typedef struct texture T;

T *texture_init(T *self,
		unsigned int type,
		const char *name) {
	self->id = SOIL_load_OGL_texture(
			name,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_POWER_OF_TWO);
	self->type = type;
	return self;
}
void texture_load(T *self) {
	glBindTexture(self->type, self->id);
}
void  texture_unload(T *self) {
	glBindTexture(self->type, 0);
}

int textures_max(void) {
	int ntex=0;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &ntex);
	return ntex;
}
