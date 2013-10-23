#include <sg/sgn_base.h>
#include <sg/ds/tzarray_p.inl>

typedef struct sgn_base T;

void sgn_base_pre_draw(T *self, struct scene *scene) {
	T **kid;
	if (self->parent) {
		/* find to_world and from_world transformation. */
		tzm4_mulm(      &sgn_base_to(self),
				&sgn_base_to(self->parent),
				&sgn_base_T(self));
		tzm4_mulm(      &sgn_base_from(self),
				&sgn_base_I(self),
				&sgn_base_from(self->parent));
	}
	tzarray_p_foreach_t(kid, &self->kids, T*) {
		sgn_pre_draw(*kid, scene);
	}
}
void sgn_base_draw(T *self, struct scene *scene) {
	T **kid;
	tzarray_p_foreach_t(kid, &self->kids, T*) {
		sgn_draw(*kid, scene);
	}
}
void sgn_base_post_draw(T *self, struct scene *scene) {
	T **kid;
	tzarray_p_foreach_t(kid, &self->kids, T*) {
		sgn_post_draw(*kid, scene);
	}
}
bool sgn_base_isvisible(T *self, struct scene *scene) {
	return true;
}
void sgn_base_addchild(T *self, T *kid) {
	kid->parent = self;
	tzarray_p_pushv(&self->kids, kid);
}
void sgn_base_update(T *self, uint16_t ms) {
	T **kid;
	tzarray_p_foreach_t(kid, &self->kids, T*) {
		sgn_update(*kid, ms);
	}
}

static struct sgn_vtbl sgn_base_vtbl = {
	.pre_draw  = sgn_base_pre_draw,
	.draw      = sgn_base_draw,
	.post_draw = sgn_base_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

void sgn_base_init(T *self, struct sgn_vtbl *vtbl) {
	self->vtbl = vtbl ? vtbl : &sgn_base_vtbl;
	self->name = NULL;
	self->parent = NULL;

	tzarray_p_init(&self->kids);
	tzm4_mkiden(&self->to);
	tzm4_mkiden(&self->from);
	tzm4_mkiden(&self->T);
	tzm4_mkiden(&self->I);
	tzm4_mkiden(&self->to);
	tzm4_mkiden(&self->from);
}
void sgn_base_setvtbl(T *self, struct sgn_vtbl *tbl) {
	self->vtbl = tbl;
}

struct sgn_vtbl *sgn_base_getvtbl(T *self) {
	return self->vtbl;
}

void sgn_base_setname(T *self, const char *name) {
	self->name = name;
}
const char *sgn_base_getname(T *self) {
	return self->name;
}

bool sgn_isbase(struct sgn_base *self) {
	return self->vtbl == &sgn_base_vtbl;
}

T *sgn_translate(T *self, tzv4 v) {
	tzm4_translate(&sgn_base_T(self),                v );
	tzm4_inverse  (&sgn_base_I(self), &sgn_base_T(self));
	return self;
}

T *sgn_rotate(T *self, double theta, tzv4 v) {
	tzm4_rotate(&sgn_base_T(self),  theta, v);
	tzm4_inverse(&sgn_base_I(self), &sgn_base_T(self));
	return self;
}

T *sgn_scale(T *self, tzv4 v) {
	tzm4_scale  (&sgn_base_T(self), v);
	tzm4_inverse(&sgn_base_I(self), &sgn_base_T(self));
	return self;
}

/* vtable boiler plate */
void sgn_pre_draw(T *self, struct scene *scene) {
	self->vtbl->pre_draw(self, scene);
}
void sgn_draw(T *self, struct scene *scene) {
	self->vtbl->draw(self, scene);
}
void sgn_post_draw(T *self, struct scene *scene) {
	self->vtbl->post_draw(self, scene);
}
bool sgn_isvisible(T *self, struct scene *scene) {
	return self->vtbl->isvisible(self, scene);
}
void sgn_addchild(T *self, struct sgn_base *child) {
	self->vtbl->addchild(self, child);
}
void sgn_update(T *self, uint16_t ms) {
	self->vtbl->update(self, ms);
}

