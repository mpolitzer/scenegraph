#ifndef SCENE_ENV_H
#define SCENE_ENV_H

#include <sg/gl/backend.h>
#include <tzmath/tzmath.h>

struct scene_env {
	tzv4 fog_color;
	float fog_density;
	float start, end;
	uint32_t mode[3];
	uint32_t filter;
};

void scene_env_init_fog   (struct scene *self);
void scene_env_enable_fog (struct scene *self);
void scene_env_disable_fog(struct scene *self);

#endif /* SCENE_ENV_H */
