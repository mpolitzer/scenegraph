#include <sg/sgn_cam.h>

typedef struct sgn_cam T;

static struct sgn_vtbl sgn_cam_vtbl = {
	.pre_draw  = sgn_base_pre_draw,
	.draw      = sgn_base_draw,
	.post_draw = sgn_base_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

struct sgn_cam *sgn_cam_init(T *self, const char *name) {
	sgn_base_init(&self->base, &sgn_cam_vtbl);
	sgn_base_name(&self->base) = name;
	return self;
}

bool sgn_iscam(struct sgn_cam *self) {
	return sgn_base_vtbl(&self->base) == &sgn_cam_vtbl;
}

void sgn_cam_setup(T *self, struct scene *scene) {
	tzm4 V, I;
	struct sgn_base *tmp;

	assert(sgn_iscam(self));
	tzm4_mkiden(&V);
	tzm4_mkiden(&I);
#if 1
	for (tmp = &self->base; tmp; tmp = tmp->parent) {
		tzm4_mulm(&V, &V, sgn_base_I(tmp));
		tzm4_mulm(&I, &I, sgn_base_T(tmp));
	}
#endif
	*sgn_base_to(&self->base)   = V;
	*sgn_base_from(&self->base) = I;
}

void sgn_cam_attach(T *self, struct sgn_base *parent) {
	sgn_base_parent(&self->base) = parent;
}
