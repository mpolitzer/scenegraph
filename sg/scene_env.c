#include <sg/scene.h>
#include <sg/scene_env.h>

void scene_env_fog_init(struct scene_env *self) {
	self->fog_color   = tzv4_mkp(0.5, 0.5, 0.5);
	self->fog_density = 0.35;
	self->start = 10.0f;
	self->end   = 20.0f;

	self->mode[0] = GL_EXP;
	self->mode[1] = GL_EXP2;
	self->mode[2] = GL_LINEAR;
	self->filter  = 0;
}

void scene_env_fog_enable(struct scene_env *self) {

	tzv4 *c = &self->fog_color;

	glClearColor(c->m.x, c->m.y, c->m.z, c->m.w);

	glFogi(GL_FOG_MODE, self->mode[self->filter]);
	glFogfv(GL_FOG_COLOR, self->fog_color.f);
	glFogf(GL_FOG_DENSITY, self->fog_density);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_START, self->start);
	glFogf(GL_FOG_END, self->end);
	glEnable(GL_FOG);
}

void scene_env_fog_disable(struct scene_env *self) {
	glDisable(GL_FOG);
}
