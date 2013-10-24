#include <sg/scene.h>
#include <sg/sgn_light.h>
#include <sg/gl/backend.h>

typedef struct sgn_light T;

/* num of lights in use */
static int _lights;

void sg_core_axes(void) {
	glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);glVertex3f(1, 0, 0);
        glColor3f(0, 1, 0); glVertex3f(0, 0, 0);glVertex3f(0, 1, 0);
        glColor3f(0, 0, 1); glVertex3f(0, 0, 0);glVertex3f(0, 0, 1);
        glEnd();
	glEnable(GL_LIGHTING);
}

void sgn_light_pre_draw(struct sgn_base *_self, struct scene *scene) {
	T *self = (T *)_self;
	tzm4 tmp;
	tzm4 *cam  = sgn_base_to(scene->active_cam);
	tzm4_mulm(&tmp, cam, sgn_base_to(_self));
	sgn_base_pre_draw(_self, scene);
	light_load(&self->light, &tmp);
	sg_core_axes();
}

void sgn_light_post_draw(struct sgn_base *_self, struct scene *scene) {
	T *self = (T *)_self;
	sgn_base_post_draw(_self, scene);
	light_unload(&self->light);
}

static struct sgn_vtbl sgn_light_vtbl = {
	.pre_draw  = sgn_light_pre_draw,
	.draw      = sgn_base_draw,
	.post_draw = sgn_light_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

T *sgn_light_init(T *self, const char *name) {
	sgn_base_init(&self->base, &sgn_light_vtbl);
	sgn_base_name(&self->base) = name;
	light_init(&self->light);
	return self;
}
