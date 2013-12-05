#ifndef SGN_GEOM_H
#define SGN_GEOM_H

#include <sg/sgn_base.h>
#include <sg/geometry.h>
#include <sg/material.h>
#include <sg/texture.h>
#include <tzmath/tzmath.h>

struct sgn_geom {
	struct sgn_base base;
	struct geometry *geom;
	struct material *mat;
	struct texture  *tex;
	tzm4 localT, localI;

	/* One update per object. */
	void (*update)(struct sgn_geom *self);
};

#define sgn_geom_localT(_self) (&(_self)->localT)
#define sgn_geom_material(_self) (&(_self)->material)
#define sgn_geom_geom(_self)   (&(_self)->geom)
#define sgn_geom_mat(_self)   (&(_self)->mat)
#define sgn_geom_tex(_self)   (&(_self)->tex)

bool
sgn_isgeom(
		struct sgn_geom *self);
void sgn_geom_init(
		struct sgn_geom *self,
		const char *name,
		struct geometry *geom,
		struct material *mat,
		struct texture *tex);
void sgn_geom_translate(
		struct sgn_geom *self,
		tzv4 v);
void sgn_geom_rotate(
		struct sgn_geom *self,
		float theta,
		tzv4 v);
void sgn_geom_scale(
		struct sgn_geom *self,
		tzv4 v);

void sgn_geom_draw(struct sgn_base *_self, struct scene *scene);

#endif /* SGN_GEOM_H */
