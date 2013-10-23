#include <sg/scene_env.h>

void scene_env_enable_fog(struct scene_env *env) {
	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, env->fog_color.f);
	glFogf(GL_FOG_DENSITY, env->fog_density);
	glHint(GL_FOG_HINT, GL_NICEST);
}

void scene_env_disable_fog(void) {
	glDisable(GL_FOG);
}
