#include <sg/scene.h>
#include <sg/sgn_geom.h>

typedef struct sgn_geom T;

/* vertex for surfaces generation. mksurface will convert to the final type. */
struct vertex {
	tzv4 pos, normal, texcoord;
};

static void sgn_geom_pre_draw(struct sgn_base *_self, struct scene *scene) {
	sgn_base_pre_draw(_self, scene);
}

static void sgn_geom_draw(struct sgn_base *_self, struct scene *scene) {
	tzm4 tmp;
	T *self = (T *)_self;
	assert(sgn_isgeom(self) && "node is not a sgn_geom, fix vtbl.");
	/* apply the local transformation. */
	tzm4_mulm(&tmp, &sgn_base_to(_self),
			&sgn_geom_localT(self));

	/* TODO: fix bull crap */
	if (self->mat) material_load(self->mat);
	geometry_draw(self->geom, &tmp);
	if (self->mat) material_unload(self->mat);

	sgn_base_draw(&self->base, scene);
}

static void sgn_geom_post_draw(struct sgn_base *_self, struct scene *scene) {
	sgn_base_post_draw(_self, scene);
}

static struct sgn_vtbl sgn_geom_vtbl = {
	.pre_draw  = sgn_geom_pre_draw,
	.draw      = sgn_geom_draw,
	.post_draw = sgn_geom_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

void sgn_geom_init(T *self, const char *name,
		struct geometry *geom,
		struct material *mat) {
	sgn_base_init(&self->base, &sgn_geom_vtbl);
	sgn_base_name(&self->base) = name;
	sgn_geom_geom(self) = geom;
	sgn_geom_mat(self)  = mat;
	tzm4_mkiden(&sgn_geom_localT(self));
}

bool sgn_isgeom(T *self) {
	return self->base.vtbl == &sgn_geom_vtbl;
}

T *sgn_geom_dup(T *self, T *src) {
	*self = *src;
	return self;
}
