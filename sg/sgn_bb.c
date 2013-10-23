#include <sg/scene.h>
#include <sg/sgn_bb.h>
#include <sg/sgn_cam.h>

typedef struct sgn_geom T;

void sgn_bb_draw(struct sgn_base *_self, struct scene *scene) {
	struct sgn_base *root = &scene->root;
	tzm4 tmp;
	T *self = (T *)_self;

	tzv4 up    = tzv4_normalize(tzm4_mulv(&sgn_base_to(root), tzv4_mkv(0, 1, 0))),
	     vdir  = tzv4_normalize(tzm4_mulv(&sgn_base_to(root), tzv4_mkv(1, 0, 0))),
	     right = tzv4_cross(up, right);
	tzm4_mkcols(&tmp,
			up,
			right,
			vdir,
			sgn_base_to(_self).c._3
			);
#if 1
	/* apply the localT transformation. */
	tzm4_mulm(&tmp,
		&tmp,
		&sgn_geom_localT(self)
			);
#endif
	/* TODO: fix bull crap */
	if (self->mat) material_load  (self->mat);
	if (self->tex) texture_load   (self->tex);
	geometry_draw(self->geom, &tmp);
	if (self->tex) texture_unload (self->tex);
	if (self->mat) material_unload(self->mat);

	sgn_base_draw(&self->base, scene);

}

static struct sgn_vtbl sgn_bb_vtbl = {
	.pre_draw  = sgn_base_pre_draw,
	.draw      = sgn_bb_draw,
	.post_draw = sgn_base_post_draw,
	.isvisible = sgn_base_isvisible,
	.addchild  = sgn_base_addchild,
	.update    = sgn_base_update,
};

void sgn_bb_init(
		struct sgn_geom *self,
		const char *name,
		struct geometry *geom,
		struct material *mat,
		struct texture *tex) {
	sgn_geom_init(self, name, geom, mat, tex);
	sgn_base_setvtbl(&self->base, &sgn_bb_vtbl);
}
