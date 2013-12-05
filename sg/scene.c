#include <sg/scene.h>

typedef struct scene T;

void scene_init(T *self) {
	sgn_base_init(&self->root, NULL);
	sgn_base_setname(&self->root, "root");
	scene_env_fog_init(&self->env);
}

void scene_draw(T *self) {

	if (!self->active_cam) {
		printf("no cam\n");
		return;
	}
	/* setup lightmaps */

	/* setup camera */
	sgn_cam_setup((struct sgn_cam *)self->active_cam, self);

	/* setup local->global transformations & light */
	sgn_pre_draw(&self->root, self);

	/* draw geometry nodes */
	sgn_draw(&self->root, self);

	/* cleanup */
	sgn_post_draw(&self->root, self);
}

void scene_update(T *self, uint16_t ms) {
	sgn_update(&self->root, ms);
}

struct sgn_base *scene_getroot(T *self) {
	return &self->root;
}

void scene_setcam(T *self, struct sgn_base *cam) {
	self->active_cam = cam;
}
